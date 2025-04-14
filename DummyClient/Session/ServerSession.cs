using Google.Protobuf;
using Google.Protobuf.Protocol;
using Protocol;
using System;
using System.Diagnostics;
using System.Net;
using System.Threading.Tasks;

public class ServerSession : PacketSession
{
    static KeyValuePair<float, float>[] targetPosList =
        {
            new KeyValuePair<float, float>(3.0f, 2.0f),
            new KeyValuePair<float, float>(5.0f, 3.0f),
            new KeyValuePair<float, float>(4.0f, 10.0f),
            new KeyValuePair<float, float>(12.0f, 7.0f)
        };
    static readonly int SetSize = 8096;

    static int SessionNumber = 1;
    public int ID { get; set; } = 0;
    public int RandomTick { get; set; } = 0;

    long _ping1 = 0;
    long _ping2 = 0;
    long _ping3 = 0;

    async Task UpdateProcess()
    {
        int loopNum = 0;
        while (true)
        {
            try
            {
                #region Echo
                {
                    Interlocked.Exchange(ref _ping1, System.Environment.TickCount64);
                    CSChatRequest packet = new CSChatRequest();
                    packet.Name = "Client";
                    packet.Msg = "Hello Server!!";
                    SendAsync(EPacketProtocol.CsChatRequest, packet);
                }
                #endregion
                await Task.Delay(500);
                #region CircularSector
                if (ID % 5 == 0)
                {
                    Interlocked.Exchange(ref _ping2, System.Environment.TickCount64);
                    CSCircularSectorSkillRequest packet = new CSCircularSectorSkillRequest();
                    packet.Theta = 60.0f;
                    packet.Radius = 5.0f;
                    packet.MyPos = new SPosition();
                    packet.MyPos.X = 2.0f;
                    packet.MyPos.Y = 1.0f;
                    packet.Forward = new SPosition();
                    packet.Forward.X = 3.0f;
                    packet.Forward.Y = 2.0f;

                    KeyValuePair<float, float> pair = targetPosList[loopNum++ % 4];
                    packet.TargetPos = new SPosition();
                    packet.TargetPos.X = pair.Key;
                    packet.TargetPos.Y = pair.Value;
                    SendAsync(EPacketProtocol.CsCircularSectorSkillRequest, packet);
                }
                #endregion
                //await Task.Delay(RandomTick);
                #region Big
                //{
                //    Interlocked.Exchange(ref _ping3, System.Environment.TickCount64);
                //    int listSize = SetSize / 48;
                //    CSBigTestRequest packet = new CSBigTestRequest();
                //    int size = packet.CalculateSize();
                //    for (int i = 0; i < listSize; i++)
                //    {
                //        BigTest t = new BigTest();
                //        t.A = Int64.MaxValue;
                //        t.B = Int64.MaxValue;
                //        t.C = Int64.MaxValue;
                //        packet.List.Add(t);
                //    }

                //    size = packet.CalculateSize();

                //    SendAsync(EPacketProtocol.CsBigTestRequest, packet);
                //}
                #endregion
                //await Task.Delay(RandomTick);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
    }
    public void CheckPing(long tick, int pingType)
    {
        long ping;
        switch(pingType)
        {
            case 1:
                ping = Interlocked.Exchange(ref _ping1, 0);
                break;
            case 2:
                ping = Interlocked.Exchange(ref _ping2, 0);
                break;
            case 3:
                ping = Interlocked.Exchange(ref _ping3, 0);
                break;
            default:
                return;
        }
        long pong = System.Environment.TickCount64 - ping;
        if (pong >= 100)
        {
            switch (pingType)
            {
                case 1:
                    Console.WriteLine($"CSChatRequest Ping ({pong})");
                    break;
                case 2:
                    Console.WriteLine($"CSCircularSectorSkillRequest Ping ({pong})");
                    break;
                case 3:
                    Console.WriteLine($"CSBigTestRequest Ping ({pong})");
                    break;
                default:
                    return;
            }
        }
    }

    #region Packet
    public override void OnConnected(EndPoint endPoint)
    {
        ServerSessionManager.Instance.Add(this);
        ID = SessionNumber++;
        Random rd = new Random();
        RandomTick = rd.Next(1000, 3000);
        Interlocked.Increment(ref Program.ConnCount);
        Console.WriteLine($"OnConnected : {endPoint}, Session Number : {ID}");
        UpdateProcess();
    }
    public override void OnDisconnected(EndPoint endPoint)
    {
        ServerSessionManager.Instance.Remove(this);
        Interlocked.Decrement(ref Program.ConnCount);
        Console.WriteLine($"OnDisconnected : {endPoint}");
    }
    public override void OnRecvPacket(ArraySegment<byte> buffer)
    {
        // TODO : packetId 대역 체크
        Interlocked.Increment(ref Program.RecvCount);
        PacketManager.Instance.OnRecvPacket(this, buffer);
    }
    public override void OnSend(int numOfBytes)
    {
        Interlocked.Increment(ref Program.SendCount);
    }
    #endregion
}
