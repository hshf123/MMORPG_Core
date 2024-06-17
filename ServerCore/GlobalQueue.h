#pragma once
#include "Singleton.h"
#include "LockQueue.h"

class JobQueue;

class GlobalQueue : public RefSingleton<GlobalQueue>
{
public:
	void									Push(std::shared_ptr<JobQueue> jobQueue);
	std::shared_ptr<JobQueue>				Pop();

private:
	LockQueue<std::shared_ptr<JobQueue>> _jobQueues;
};
