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
        ServerSession? ss = session as ServerSession;
        if (pkt == null || ss == null)
            return;
        //Console.WriteLine($"OnSCChatResponse");
        
    }

    public static void OnSCCircularSectorSkillResponse(PacketSession session, IMessage packet)
    {
        SCCircularSectorSkillResponse? pkt = packet as SCCircularSectorSkillResponse;
        ServerSession? ss = session as ServerSession;
        if (pkt == null || ss == null)
            return;

        //Console.WriteLine($"OnSCCircularSectorSkillResponse");
    }

    public static void OnSCBigTestResponse(PacketSession session, IMessage packet)
    {
        SCBigTestResponse? pkt = packet as SCBigTestResponse;
        ServerSession? ss = session as ServerSession;
        if (pkt == null || ss == null)
            return;
        //Console.WriteLine($"OnSCBigTestResponse");
    }
}
