using Google.Protobuf;
using Protocol;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class ServerSessionManager
{
    static ServerSessionManager _instance = new ServerSessionManager();
    public static ServerSessionManager Instance { get { return _instance; } }

    List<ServerSession> _sessions = new List<ServerSession>();
    //object _lock = new object();
    AsyncLock _lock = new AsyncLock();

    public async Task AddAsync(ServerSession session)
    {
        using (await _lock.LockAsync())
        {
            _sessions.Add(session);
        }
    }

    public async Task RemoveAsync(ServerSession session)
    {
        using (await _lock.LockAsync())
        {
            _sessions.Remove(session);
        }
    }

    public async Task BroadcastAsync(EPacketProtocol protocol, IMessage packet)
    {
        List<ServerSession> sessions;
        using (await _lock.LockAsync())
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

    public async Task<ServerSession> CreateSessionAsync()
    {
        ServerSession session = new ServerSession();
        using (await _lock.LockAsync())
        {
            _sessions.Add(session);
        }
        return session;
    }
}
