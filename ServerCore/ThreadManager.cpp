#include "pch.h"
#include "ThreadManager.h"
#include "TLS.h"

ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{

}

void ThreadManager::Launch(std::function<void(void)> fn)
{
	WRITE_LOCK;
	_threads.push_back(std::jthread([=]()
		{
			fn();
		}));
}

void ThreadManager::InitTLS()
{
	static std::atomic_uint SThreadId = 1;
	LThreadId = SThreadId++;
}
