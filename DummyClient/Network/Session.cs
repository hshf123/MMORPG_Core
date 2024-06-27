using Google.Protobuf;
using Protocol;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

public abstract class PacketSession : Session
{
    public static readonly int HeaderSize = 2;

    public sealed override int OnRecv(ArraySegment<byte> buffer)
    {
        int processLen = 0;

        while (true)
        {
            // 최소한 헤더는 파싱할 수 있는지 확인
            if (buffer.Count < HeaderSize)
                break;

            // 패킷이 완전체로 도착했는지 확인
            ushort dataSize = BitConverter.ToUInt16(buffer.Array!, buffer.Offset);
            if (buffer.Count < dataSize)
                break;

            // 여기까지 왔으면 패킷 조립 가능
            OnRecvPacket(new ArraySegment<byte>(buffer.Array!, buffer.Offset, dataSize));

            processLen += dataSize;
            buffer = new ArraySegment<byte>(buffer.Array!, buffer.Offset + dataSize, buffer.Count - dataSize);
        }

        return processLen;
    }

    public abstract void OnRecvPacket(ArraySegment<byte> buffer);

    public async Task SendAsync(EPacketProtocol protocol, IMessage packet)
    {
        ushort size = (ushort)packet.CalculateSize();
        byte[] sendBuffer = new byte[size + 4];
        Array.Copy(BitConverter.GetBytes((ushort)(size + 4)), 0, sendBuffer, 0, sizeof(ushort));
        Array.Copy(BitConverter.GetBytes((ushort)protocol), 0, sendBuffer, 2, sizeof(ushort));
        Array.Copy(packet.ToByteArray(), 0, sendBuffer, 4, size);
        await SendAsync(new ArraySegment<byte>(sendBuffer));
    }
}

public abstract class Session
{
    Socket? _socket;
    int _disconnected = 0;

    RecvBuffer _recvBuffer = new RecvBuffer();

    //object _lock = new object();
    AsyncLock _lock = new AsyncLock();
    Queue<ArraySegment<byte>> _sendQueue = new Queue<ArraySegment<byte>>();
    List<ArraySegment<byte>> _pendingList = new List<ArraySegment<byte>>();

    public abstract void OnConnected(EndPoint endPoint);
    public abstract int OnRecv(ArraySegment<byte> buffer);
    public abstract void OnSend(int numOfBytes);
    public abstract void OnDisconnected(EndPoint endPoint);

    async void Clear()
    {
        using (await _lock.LockAsync())
        {
            _sendQueue.Clear();
            _pendingList.Clear();
        }
    }

    public async Task Start(Socket? socket)
    {
        if (socket == null)
            return;
        _socket = socket;
        await RegisterRecv();
    }

    public async Task Send(List<ArraySegment<byte>> sendBuffList)
    {
        if (sendBuffList.Count == 0)
            return;

        using (await _lock.LockAsync())
        {
            foreach (ArraySegment<byte> sendBuff in sendBuffList)
                _sendQueue.Enqueue(sendBuff);
        }

        if (_pendingList.Count == 0)
            await RegisterSend();
    }

    public async Task SendAsync(ArraySegment<byte> sendBuff)
    {
        using (await _lock.LockAsync())
        {
            _sendQueue.Enqueue(sendBuff);
            if (_pendingList.Count == 0)
                await RegisterSend();
        }
    }

    public void Disconnect()
    {
        if (Interlocked.Exchange(ref _disconnected, 1) == 1)
            return;

        OnDisconnected(_socket!.RemoteEndPoint!);
        _socket.Shutdown(SocketShutdown.Both);
        _socket.Close();
        Clear();
    }

    #region 네트워크 통신

    async Task RegisterSend()
    {
        if (_disconnected == 1)
            return;

        while (_sendQueue.Count > 0)
        {
            ArraySegment<byte> buff = _sendQueue.Dequeue();
            _pendingList.Add(buff);
        }

        int sendLen = 0;
        try
        {
            sendLen = await _socket!.SendAsync(_pendingList, SocketFlags.None);
        }
        catch (Exception e)
        {
            //Debug.Log($"RegisterSend Failed {e}");
            Console.WriteLine($"RegisterSend Failed {e}");
            return;
        }

        await OnSendCompleted(sendLen);
    }

    async Task OnSendCompleted(int sendLen)
    {
        // 위에서 락 잡고 들어옴
        if (sendLen > 0)
        {
            try
            {
                _pendingList.Clear();

                OnSend(sendLen);

                if (_sendQueue.Count > 0)
                    await RegisterSend();
            }
            catch (Exception e)
            {
                //Debug.Log($"OnSendCompleted Failed {e}");
                Console.WriteLine($"OnSendCompleted Failed {e}");
            }
        }
        else
        {
            Disconnect();
        }
    }

    async Task RegisterRecv()
    {
        if (_disconnected == 1)
            return;

        _recvBuffer.Clean();
        ArraySegment<byte> segment = _recvBuffer.WriteSegment;
        int recvLen;
        try
        {
            recvLen = await _socket!.ReceiveAsync(segment.Array!, SocketFlags.None);
        }
        catch (Exception e)
        {
            //Debug.Log($"RegisterRecv Failed {e}");
            Console.WriteLine($"RegisterRecv Failed {e}");
            return;
        }

        OnRecvCompletedAsync(recvLen);
    }

    void OnRecvCompletedAsync(int recvLen)
    {
        if (recvLen > 0)
        {
            try
            {
                // Write 커서 이동
                if (_recvBuffer.OnWrite(recvLen) == false)
                {
                    Disconnect();
                    return;
                }

                // 컨텐츠 쪽으로 데이터를 넘겨주고 얼마나 처리했는지 받는다
                int processLen = OnRecv(_recvBuffer.ReadSegment);
                if (processLen < 0 || _recvBuffer.DataSize < processLen)
                {
                    Disconnect();
                    return;
                }

                // Read 커서 이동
                if (_recvBuffer.OnRead(processLen) == false)
                {
                    Disconnect();
                    return;
                }

#pragma warning disable CS4014 // 이 호출을 대기하지 않으므로 호출이 완료되기 전에 현재 메서드가 계속 실행됩니다.
                RegisterRecv();
#pragma warning restore CS4014 // 이 호출을 대기하지 않으므로 호출이 완료되기 전에 현재 메서드가 계속 실행됩니다.
            }
            catch (Exception e)
            {
                //Debug.Log($"OnRecvCompleted Failed {e}");
                Console.WriteLine($"OnRecvCompleted Failed {e}");
            }
        }
        else
        {
            Disconnect();
        }
    }

    #endregion
}
