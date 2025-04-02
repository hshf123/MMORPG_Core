using Google.Protobuf.Protocol;
using Protocol;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

public class Program
{
    static readonly int ConnectCount = 5000;
    static readonly int ConnectCountInOnce = 150;
    static readonly int TickTerm = 500;

    public static long ConnCount = 0;
    public static long SendCount = 0;
    public static long RecvCount = 0;

    static void Main(string[] args)
    {
        Thread.Sleep(3000);
        Connector connector = new Connector();
        while (true)
        {
            long connCount = Interlocked.Read(ref ConnCount);
            if (connCount < ConnectCount)
            {
                connector.Connect
                    (
                        "127.0.0.1",                                                                                         // IP
                        9999,                                                                                                // Port
                        () =>
                        {
                            return ServerSessionManager.Instance.CreateSession();                                            // Session Create Func Ptr
                        },
                        (int)(ConnectCount - connCount > ConnectCountInOnce ? ConnectCountInOnce : ConnectCount - connCount) // Dummy Client Count
                    );
                Thread.Sleep(TickTerm);
                continue;
            }

            Thread.Sleep(60000);

            long sendCount = Interlocked.Exchange(ref SendCount, 0);
            long recvCount = Interlocked.Exchange(ref RecvCount, 0);
            Console.WriteLine($"Client Send({Interlocked.Read(ref sendCount)}) Recv({Interlocked.Read(ref recvCount)})");
        }
    }
}
