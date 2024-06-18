#pragma once

#define USE_LOCK			std::shared_mutex _lock
#define USE_LOCKS(idx)		std::shared_mutex _locks[idx]
#define READ_LOCK			std::shared_lock readLock(_lock)
#define WRITE_LOCK			std::unique_lock writeLock(_lock)
#define READ_LOCKS(idx)		std::shared_lock readLock##idx(_locks[idx])
#define WRITE_LOCKS(idx)	std::unique_lock writeLock##idx(_locks[idx])

#define VIEW_INFO(fmt, ...)				LogManager::GetInstance().Log(LogType::Info, true, false, fmt, __VA_ARGS__)
#define WRITE_INFO(fmt, ...)			LogManager::GetInstance().Log(LogType::Info, false, true, fmt, __VA_ARGS__)
#define VIEW_WRITE_INFO(fmt, ...)		LogManager::GetInstance().Log(LogType::Info, true, true, fmt, __VA_ARGS__)

#define VIEW_WARNING(fmt, ...)			LogManager::GetInstance().Log(LogType::Warning, true, false, fmt, __VA_ARGS__)
#define WRITE_WARNING(fmt, ...)			LogManager::GetInstance().Log(LogType::Warning, false, true, fmt, __VA_ARGS__)
#define VIEW_WRITE_WARNING(fmt, ...)	LogManager::GetInstance().Log(LogType::Warning, true, true, fmt, __VA_ARGS__)

#define VIEW_ERROR(fmt, ...)			LogManager::GetInstance().Log(LogType::Error, true, false, fmt, __VA_ARGS__)
#define WRITE_ERROR(fmt, ...)			LogManager::GetInstance().Log(LogType::Error, false, true, fmt, __VA_ARGS__)
#define VIEW_WRITE_ERROR(fmt, ...)		LogManager::GetInstance().Log(LogType::Error, true, true, fmt, __VA_ARGS__)

#ifdef DEV_TEST
#define VIEW_INFOD(fmt, ...)
#define VIEW_WARNINGD(fmt, ...)
#define VIEW_ERRORD(fmt, ...)
#endif

#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assert(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}											

#define ASSERT_CRASH(expr)					\
{											\
	if ((expr) == false)					\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assert(expr);			\
	}										\
}