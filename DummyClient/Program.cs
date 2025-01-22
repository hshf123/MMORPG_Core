using Google.Protobuf.Protocol;
using Protocol;
using System.Net;
using System.Net.Sockets;

namespace DummyClient
{
    internal class Program
    {
        static readonly int SetSize = 8096;
        //static int pakcetNum = 0;

        static KeyValuePair<float, float>[] targetPosList =
        {
            new KeyValuePair<float, float>(3.0f, 2.0f),
            new KeyValuePair<float, float>(5.0f, 3.0f),
            new KeyValuePair<float, float>(4.0f, 10.0f),
            new KeyValuePair<float, float>(12.0f, 7.0f)
        };

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
                1                                                                    // Dummy Client Count
            );

            while (true)
            {
                try
                {
                    #region Echo
                    //CSChatRequest packet = new CSChatRequest();
                    //packet.Messageid = EPacketProtocol.CsChatRequest;
                    //packet.Name = "Client";
                    //packet.Msg = $"Hello Server {pakcetNum++}";
                    //await ServerSessionManager.Instance.BroadcastAsync(EPacketProtocol.CsChatRequest, packet);
                    #endregion
                    #region CircularSector
                    //CSCircularSectorSkillRequest packet = new CSCircularSectorSkillRequest();
                    //packet.Theta = 60.0f;
                    //packet.Radius = 5.0f;
                    //packet.MyPos = new SPosition();
                    //packet.MyPos.X = 2.0f;
                    //packet.MyPos.Y = 1.0f;
                    //packet.Forward = new SPosition();
                    //packet.Forward.X = 3.0f;
                    //packet.Forward.Y = 2.0f;

                    //KeyValuePair<float, float> pair = targetPosList[pakcetNum++ % 4];
                    //packet.TargetPos = new SPosition();
                    //packet.TargetPos.X = pair.Key;
                    //packet.TargetPos.Y = pair.Value;
                    //await ServerSessionManager.Instance.BroadcastAsync(EPacketProtocol.CsCircularSectorSkillRequest, packet);
                    #endregion
                    #region Big
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

                    await ServerSessionManager.Instance.BroadcastAsync(EPacketProtocol.CsBigTestRequest, packet);
                    #endregion
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }

                Thread.Sleep(1000);
            }
        }
    }
}
