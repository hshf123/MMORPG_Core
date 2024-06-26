using Google.Protobuf;
using System;
using System.Diagnostics;
using System.Net;

public class ServerSession : PacketSession
{
    public override void OnConnected(EndPoint endPoint)
    {
        //Debug.Log($"OnConnected : {endPoint}");
    }

    public override void OnDisconnected(EndPoint endPoint)
    {
        //Debug.Log($"OnDisconnected : {endPoint}");
    }

    public override void OnRecvPacket(ArraySegment<byte> buffer)
    {
        // TODO : packetId 대역 체크
        //PacketManager.Instance.OnRecvPacket(this, buffer);
    }

    public override void OnSend(int numOfBytes)
    {
    }
}
