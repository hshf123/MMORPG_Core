using Google.Protobuf;
using Google.Protobuf.Protocol;
using Protocol;
using System;
using System.Diagnostics;
using System.Net;
using System.Numerics;

public class PacketHandler
{
    public static void OnSCChatResponse(PacketSession session, IMessage packet)
    {
        SCChatResponse? pkt = packet as SCChatResponse;
        if (pkt == null)
            return;

        //Console.WriteLine($"RecvFrom Server {pkt.Name} : {pkt.Msg}");
    }
}
