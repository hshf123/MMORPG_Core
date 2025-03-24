using Google.Protobuf.Protocol;
using Protocol;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace DummyClient
{
    internal class Program
    {
        static readonly int ConnectCount = 5000;
        static readonly int ConnectCountInOnce = 100;
        static readonly int TickTerm = 3000;

        static async Task Main(string[] args)
        {
            await Task.Delay(TickTerm);
            Connector connector = new Connector();
            #region Connect
            int connCount = 0;
            long tick = System.Environment.TickCount64;
            #endregion
            while (true)
            {
                if (connCount < ConnectCount)
                {
                    connector.Connect
                        (
                            "127.0.0.1",                                                            // IP
                            9999,                                                                   // Port
                            () =>
                            {
                                return ServerSessionManager.Instance.CreateSession();               // Session Create Func Ptr
                            },
                            ConnectCountInOnce                                                      // Dummy Client Count
                        );
                    connCount += ConnectCountInOnce;
                }

                await Task.Delay(TickTerm);
            }
        }
    }
}
