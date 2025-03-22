using Google.Protobuf;
using Google.Protobuf.Protocol;
using Protocol;
using System;
using System.Diagnostics;
using System.Net;

public class ServerSession : PacketSession
{
    static int SessionNumber = 1;
    int SessionID { get; set; } = 0;

    static KeyValuePair<float, float>[] targetPosList =
        {
            new KeyValuePair<float, float>(3.0f, 2.0f),
            new KeyValuePair<float, float>(5.0f, 3.0f),
            new KeyValuePair<float, float>(4.0f, 10.0f),
            new KeyValuePair<float, float>(12.0f, 7.0f)
        };
    int loopNum = 0;
    static readonly int SetSize = 8096;

    #region Packet
    public override void OnConnected(EndPoint endPoint)
    {
        ServerSessionManager.Instance.Add(this);
        Console.WriteLine($"OnConnected : {endPoint}, Session Number : {SessionNumber++}");
        SessionID = SessionNumber;
        TaskManager.Instance.AddTask(UpdateProcessAsync());
    }
    public override void OnDisconnected(EndPoint endPoint)
    {
        ServerSessionManager.Instance.Remove(this);
        Console.WriteLine($"OnDisconnected : {endPoint}");
    }
    public override void OnRecvPacket(ArraySegment<byte> buffer)
    {
        // TODO : packetId 대역 체크
        PacketManager.Instance.OnRecvPacket(this, buffer);
    }
    public override void OnSend(int numOfBytes)
    {
    }
    #endregion

    public async Task UpdateProcessAsync()
    {
        #region Echo
        {
            CSChatRequest packet = new CSChatRequest();
            packet.Name = "Client";
            packet.Msg = "Hello Server!!";
            SendAsync(EPacketProtocol.CsChatRequest, packet);
        }
        #endregion
        await Task.Delay(1000 + SessionID);
        #region CircularSector
        {
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
        await Task.Delay(1000 + SessionID);
        #region Big
        {
            int listSize = SetSize / 24;

            CSBigTestRequest packet = new CSBigTestRequest();
            int size = packet.CalculateSize();
            for (int i = 0; i < listSize; i++)
            {
                BigTest t = new BigTest();
                t.A = Int64.MaxValue;
                t.B = Int64.MaxValue;
                t.C = Int64.MaxValue;
                packet.List.Add(t);
            }

            size = packet.CalculateSize();
            SendAsync(EPacketProtocol.CsBigTestRequest, packet);
        }
        #endregion
        await Task.Delay(1000 + SessionID);
        TaskManager.Instance.AddTask(UpdateProcessAsync());
    }
}
