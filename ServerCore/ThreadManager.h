#pragma once
#include "Singleton.h"

class ThreadManager : public RefSingleton<ThreadManager>
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(std::function<void(void)> fn);

	static void InitTLS();

	static void DoGlobalQueueWork();
	static void DistributeReservedJobs();

private:
	USE_LOCK;
	std::vector<std::jthread> _threads;
};

