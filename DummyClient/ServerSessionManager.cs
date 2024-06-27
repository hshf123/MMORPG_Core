using Google.Protobuf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class ServerSessionManager
{
    List<ServerSession> _sessions = new List<ServerSession>();
    //object _lock = new object();
    AsyncLock _lock = new AsyncLock();

    public void Add(ServerSession session)
    {
        using (_lock.LockAsync())
        {
            _sessions.Add(session);
        }
    }

    public void Remove(ServerSession session)
    {
        using (_lock.LockAsync())
        {
            _sessions.Remove(session);
        }
    }

    public async Task BroadcastAsync(IMessage packet)
    {
        using (_lock.LockAsync())
        {
            foreach (ServerSession session in _sessions)
            {
                await session.SendAsync(packet);
            }
        }
    }
}
