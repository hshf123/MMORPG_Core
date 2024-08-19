using Google.Protobuf.Protocol;
using Protocol;
using System.Net;
using System.Net.Sockets;

namespace DummyClient
{
    internal class Program
    {
        static int pakcetNum = 1;
        static async Task Main(string[] args)
        {
            Thread.Sleep(1000);


            Connector connector = new Connector();
            connector.Connect(
                "127.0.0.1",                                                            // IP
                9999,                                                                   // Port
                () =>
                {
                    return ServerSessionManager.Instance.CreateSession();               // Session Create Func Ptr
                },
                15000                                                                    // Dummy Client Count
            );

            while (true)
            {
                try
                {
                    CSChatRequest packet = new CSChatRequest();
                    packet.Messageid = EPacketProtocol.CsChatRequest;
                    packet.Name = "Client";
                    packet.Msg = $"Hello Server {pakcetNum++}";
                    await ServerSessionManager.Instance.BroadcastAsync(EPacketProtocol.CsChatRequest, packet);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }

                Thread.Sleep(250);
            }
        }
    }
}
