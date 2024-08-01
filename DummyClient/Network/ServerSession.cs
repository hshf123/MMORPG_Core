using Google.Protobuf;
using System;
using System.Diagnostics;
using System.Net;

public class ServerSession : PacketSession
{
    static int SessionNumber = 1;

    public override void OnConnected(EndPoint endPoint)
    {
        ServerSessionManager.Instance.Add(this);
        //Debug.Log($"OnConnected : {endPoint}");
        Console.WriteLine($"OnConnected : {endPoint}, Session Number : {SessionNumber++}");
    }

    public override void OnDisconnected(EndPoint endPoint)
    {
        ServerSessionManager.Instance.Remove(this);
        //Debug.Log($"OnDisconnected : {endPoint}");
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
}
