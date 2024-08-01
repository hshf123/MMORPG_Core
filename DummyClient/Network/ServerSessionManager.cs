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
    Lock _lock = new Lock();

    public void Add(ServerSession session)
    {
        using (WriteLock wLock = new WriteLock(_lock))
        {
            _sessions.Add(session);
        }
    }

    public void Remove(ServerSession session)
    {
        using (WriteLock wLock = new WriteLock(_lock))
        {
            _sessions.Remove(session);
        }
    }

    public async Task BroadcastAsync(EPacketProtocol protocol, IMessage packet)
    {
        List<ServerSession> sessions;
        using (WriteLock wLock = new WriteLock(_lock))
        {
            sessions = _sessions.ToList();
        }

        foreach (ServerSession session in sessions)
        {
            try
            {
                await session.SendAsync(protocol, packet);
            }
            catch (Exception ex)
            {
                // Log the exception or handle it accordingly
                Console.WriteLine($"Failed to send packet to session: {ex.Message}");
            }
        }
    }

    public ServerSession CreateSession()
    {
        ServerSession session = new ServerSession();
        return session;
    }
}
