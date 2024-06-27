using Google.Protobuf.Protocol;
using Protocol;
using System.Net;
using System.Net.Sockets;

namespace DummyClient
{
    internal class Program
    {
        static async Task Main(string[] args)
        {
            Thread.Sleep(5000);

            Connector connector = new Connector();
            connector.Connect("127.0.0.1", 9999, async () => { return await ServerSessionManager.Instance.CreateSessionAsync(); }, 1);

            while (true)
            {
                try
                {
                    CSChatRequest packet = new CSChatRequest();
                    packet.Messageid = EPacketProtocol.CsChatRequest;
                    packet.Name = "Client";
                    packet.Msg = "Hello Server";
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
