#pragma once

#define USE_LOCK			std::shared_mutex _lock
#define USE_LOCKS(idx)		std::shared_mutex _locks[idx]
#define READ_LOCK			std::shared_lock readLock(_lock)
#define WRITE_LOCK			std::unique_lock writeLock(_lock)
#define READ_LOCKS(idx)		std::shared_lock readLock##idx(_locks[idx])
#define WRITE_LOCKS(idx)	std::unique_lock writeLock##idx(_locks[idx])
