using DummyClient;
using System;
using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

public class Connector
{
    Func<Session>? _sessionFactory;
    public void Connect(string serverIP, int serverPort, Func<Session> sessionFactory, int count = 1)
    {
        for (int i = 0; i < count; i++)
        {
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            _sessionFactory = sessionFactory;

            SocketAsyncEventArgs args = new SocketAsyncEventArgs();
            args.RemoteEndPoint = new IPEndPoint(IPAddress.Parse(serverIP), serverPort);
            args.UserToken = socket;
#pragma warning disable CS4014 // 이 호출을 대기하지 않으므로 호출이 완료되기 전에 현재 메서드가 계속 실행됩니다.
            RegisterConnect(args);
#pragma warning restore CS4014 // 이 호출을 대기하지 않으므로 호출이 완료되기 전에 현재 메서드가 계속 실행됩니다.
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
            Console.WriteLine($"{e}");
            return;
        }
        OnConnectCompletedAsync(args);
    }

    void OnConnectCompletedAsync(SocketAsyncEventArgs args)
    {
        if (args.SocketError == SocketError.Success)
        {
            Session session = _sessionFactory!.Invoke();
#pragma warning disable CS4014 // 이 호출을 대기하지 않으므로 호출이 완료되기 전에 현재 메서드가 계속 실행됩니다.
            session.Start(args.UserToken as Socket);
#pragma warning restore CS4014 // 이 호출을 대기하지 않으므로 호출이 완료되기 전에 현재 메서드가 계속 실행됩니다.
            session.OnConnected(args.RemoteEndPoint!);
        }
        else
        {
            //Debug.Log($"OnConnectCompleted Fail: {args.SocketError}");
            Console.WriteLine($"OnConnectCompleted Fail: {args.SocketError}");
        }
    }
}
