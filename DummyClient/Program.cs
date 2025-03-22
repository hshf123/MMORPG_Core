using Google.Protobuf.Protocol;
using Protocol;
using System.Net;
using System.Net.Sockets;

namespace DummyClient
{
    internal class Program
    {
        static readonly int ConnectCount = 10;
        static readonly int ConnectCountInOnce = 1;
        static readonly int TickTerm = 3000;

        static void Main(string[] args)
        {
            Thread.Sleep(TickTerm);

            Connector connector = new Connector();
            #region Connect
            int connCount = 0;
            long tick = System.Environment.TickCount64;
            while (connCount < ConnectCount)
            {
                if (tick > System.Environment.TickCount64)
                    continue;
                tick = System.Environment.TickCount64 + 1500;

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
            }
            #endregion
            
            while (true)
            {
                int processCount = TaskManager.Instance.RunTask(100);
                if (processCount == 0)
                    continue;
                Console.WriteLine($"ProcessCount ({processCount})");
            }
        }
    }
}
