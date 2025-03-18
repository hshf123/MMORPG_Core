#include "pch.h"
#include "ThreadManager.h"
#include "TLS.h"
#include "GlobalQueue.h"
#include "JobQueue.h"
#include "DBHandler.h"

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
			InitTLS();
			fn();
		}));
}

void ThreadManager::InitTLS()
{
	static std::atomic_uint SThreadId = 1;
	LThreadId = SThreadId++;
}

void ThreadManager::DoGlobalQueueWork()
{
	while (true)
	{
		uint64 now = TimeUtils::GetTick64();
		if (now > LEndTickCount)
			break;

		// DB 결과 처리 우선으로
		std::vector<std::shared_ptr<DBData>> items;
		GlobalQueue::GetInstance().PopAllDBData(OUT items);
		if (items.empty() == false)
		{
			for (std::shared_ptr<DBData> data : items)
			{
				if (data->Response == nullptr)
					continue;

				data->Response(data);
			}

			continue;
		}

		std::shared_ptr<JobQueue> jobQueue = GlobalQueue::GetInstance().Pop();
		if (jobQueue == nullptr)
			break;

		jobQueue->Execute();
	}
}

void ThreadManager::DistributeReservedJobs()
{
	const uint64 now = TimeUtils::GetTick64();
	JobTimer::GetInstance().Distribute(now);
}
