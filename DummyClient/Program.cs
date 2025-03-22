using Google.Protobuf.Protocol;
using Protocol;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace DummyClient
{
    internal class Program
    {
        static readonly int ConnectCount = 1;
        static readonly int ConnectCountInOnce = 1;
        static readonly int TickTerm = 3000;

        static async Task Main(string[] args)
        {
            await Task.Delay(TickTerm);
            Connector connector = new Connector();
            #region Connect
            int connCount = 0;
            long tick = System.Environment.TickCount64;
            while (connCount < ConnectCount)
            {
                connector.Connect(
                    "127.0.0.1",                                                            // IP
                    9999,                                                                   // Port
                    () =>
                    {
                        return ServerSessionManager.Instance.CreateSession();               // Session Create Func Ptr
                    },
                    ConnectCountInOnce                                                      // Dummy Client Count
                );
                connCount += ConnectCountInOnce;
                await Task.Delay(TickTerm);
            }
            #endregion
            while (true)
                await Task.Delay(TickTerm);
        }
    }
}
