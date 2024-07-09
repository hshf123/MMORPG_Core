#pragma once

#define USE_LOCKS(idx)		Lock _lock[idx]
#define READ_LOCKS(idx)		ReadLockGuard readLockGuard_##idx(_lock[idx], typeid(this).name())
#define WRITE_LOCKS(idx)	WriteLockGuard writeLockGuard_##idx(_lock[idx], typeid(this).name())
#define USE_LOCK			USE_LOCKS(1)
#define READ_LOCK			READ_LOCKS(0)
#define WRITE_LOCK			WRITE_LOCKS(0)


#define VIEW_INFO(time, fmt, ...)				LogManager::GetInstance().Log(time, LogType::Info, true, false, fmt, __VA_ARGS__)
#define WRITE_INFO(time, fmt, ...)				LogManager::GetInstance().Log(time, LogType::Info, false, true, fmt, __VA_ARGS__)
#define VIEW_WRITE_INFO(time, fmt, ...)			LogManager::GetInstance().Log(time, LogType::Info, true, true, fmt, __VA_ARGS__)

#define VIEW_WARNING(time, fmt, ...)			LogManager::GetInstance().Log(time, LogType::Warning, true, false, fmt, __VA_ARGS__)
#define WRITE_WARNING(time, fmt, ...)			LogManager::GetInstance().Log(time, LogType::Warning, false, true, fmt, __VA_ARGS__)
#define VIEW_WRITE_WARNING(time, fmt, ...)		LogManager::GetInstance().Log(time, LogType::Warning, true, true, fmt, __VA_ARGS__)

#define VIEW_ERROR(time, fmt, ...)				LogManager::GetInstance().Log(time, LogType::Error, true, false, fmt, __VA_ARGS__)
#define WRITE_ERROR(time, fmt, ...)				LogManager::GetInstance().Log(time, LogType::Error, false, true, fmt, __VA_ARGS__)
#define VIEW_WRITE_ERROR(time, fmt, ...)		LogManager::GetInstance().Log(time, LogType::Error, true, true, fmt, __VA_ARGS__)

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