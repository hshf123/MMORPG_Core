using System;
using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

public class Connector
{
    static int _connectFailCount = 0;

    Func<Session>? _sessionFactory;
    public void Connect(string serverIP, int serverPort, Func<Session> sessionFactory, int count = 1)
    {
        Interlocked.Exchange(ref Program.TryConnCount, Interlocked.Read(ref Program.TryConnCount) + count);
        for (int i = 0; i < count; i++)
        {
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            _sessionFactory = sessionFactory;

            SocketAsyncEventArgs args = new SocketAsyncEventArgs();
            args.RemoteEndPoint = new IPEndPoint(IPAddress.Parse(serverIP), serverPort);
            args.UserToken = socket;
            RegisterConnect(args);
        }
    }

    async Task RegisterConnect(SocketAsyncEventArgs args)
    {
        Socket? socket = args.UserToken as Socket;
        if (socket == null)
            return;
        try
        {
            if (args.RemoteEndPoint != null)
                await socket.ConnectAsync(args.RemoteEndPoint);
        }
        catch (SocketException e)
        {
            //Debug.Log(e.ToString());
            //Console.WriteLine($"{e}");
            Console.WriteLine($"FailCount : {++_connectFailCount}");
            Interlocked.Decrement(ref Program.TryConnCount);
            return;
        }
        Interlocked.Decrement(ref Program.TryConnCount);
        OnConnectCompletedAsync(args);
    }

    void OnConnectCompletedAsync(SocketAsyncEventArgs args)
    {
        if (args.SocketError == SocketError.Success)
        {
            Session session = _sessionFactory!.Invoke();
            session.Start(args.UserToken as Socket);
            session.OnConnected(args.RemoteEndPoint!);
        }
        else
        {
            //Debug.Log($"OnConnectCompleted Fail: {args.SocketError}");
            Console.WriteLine($"OnConnectCompleted Fail: {args.SocketError}");
        }
    }
}
