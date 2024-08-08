using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Lock
{
    const long ACQUIRE_TIMEOUT_TICK = 10000;
    const uint MAX_SPIN_COUNT = 5000;
    const ulong WRITE_THREAD_MASK = 0xFFFFFFFF00000000;
    const ulong READ_COUNT_MASK = 0x00000000FFFFFFFF;
    const ulong EMPTY_FLAG = 0x000000000000000;

    ulong _lockFlag = EMPTY_FLAG;
    ushort _writeCount = 0;

    public void WriteLock()
    {
        ulong lockThreadId = Interlocked.Read(ref _lockFlag) >> 32;
        if((ulong)Thread.CurrentThread.ManagedThreadId == lockThreadId)
        {
            _writeCount++;
            return;
        }

        long beginTick = Environment.TickCount64;
        ulong desired = ((ulong)Thread.CurrentThread.ManagedThreadId << 32) & WRITE_THREAD_MASK;
        while (true)
        {
            for (uint spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
            {
                if(Interlocked.CompareExchange(ref _lockFlag, desired, EMPTY_FLAG) == EMPTY_FLAG)
                {
                    _writeCount++;
                    return;
                }
            }

            if (Environment.TickCount64 - beginTick > ACQUIRE_TIMEOUT_TICK)
                Environment.Exit(0);

            Thread.Yield();
        }
    }
    public void WriteUnLock()
    {
        if ((Interlocked.Read(ref _lockFlag) & READ_COUNT_MASK) != 0)
            Environment.Exit(0);

        ushort lockCount = --_writeCount;
        if (lockCount == 0)
            Interlocked.Exchange(ref _lockFlag, EMPTY_FLAG);
    }
    public void ReadLock()
    {
        ulong lockThreadId = (Interlocked.Read(ref _lockFlag) & WRITE_THREAD_MASK) >> 32;
        if (lockThreadId == (ulong)Thread.CurrentThread.ManagedThreadId)
        {
            Interlocked.Increment(ref _lockFlag);
            return;
        }

        long beginTick = Environment.TickCount64;
        while (true)
        {
            for (uint spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
            {
                ulong expected = (Interlocked.Read(ref _lockFlag) & READ_COUNT_MASK);

                if (Interlocked.CompareExchange(ref _lockFlag, expected + 1, expected) == expected)
                    return;
            }

            if (Environment.TickCount64 - beginTick > ACQUIRE_TIMEOUT_TICK)
                Environment.Exit(0);

            Thread.Yield();
        }
    }
    public void ReadUnLock()
    {
        if ((Interlocked.Decrement(ref _lockFlag) & READ_COUNT_MASK) == 0)
            Environment.Exit(0);
    }
}

public class ReadLock : IDisposable
{
    private Lock _lock;

    public ReadLock(Lock rwLock)
    {
        _lock = rwLock;
        _lock.ReadLock();
    }

    public void Dispose()
    {
        _lock.ReadUnLock();
    }
}

public class WriteLock : IDisposable
{
    private Lock _lock;

    public WriteLock(Lock rwLock)
    {
        _lock = rwLock;
        _lock.WriteLock();
    }

    public void Dispose()
    {
        _lock.WriteUnLock();
    }
}
