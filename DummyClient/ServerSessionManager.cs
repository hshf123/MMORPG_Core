using Google.Protobuf;
using Protocol;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

public class ServerSessionManager
{
    static ServerSessionManager _instance = new ServerSessionManager();
    public static ServerSessionManager Instance { get { return _instance; } }

    List<ServerSession> _sessions = new List<ServerSession>();

    public void Add(ServerSession session)
    {
        _sessions.Add(session);
    }

    public void Remove(ServerSession session)
    {
        _sessions.Remove(session);
    }

    public ServerSession CreateSession()
    {
        ServerSession session = new ServerSession();
        return session;
    }
}
