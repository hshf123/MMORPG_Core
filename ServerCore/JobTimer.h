#pragma once
#include "Singleton.h"

class Job;
class JobData;

class JobData
{
public:
	JobData(std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job) : owner(owner), job(job)
	{

	}

	std::weak_ptr<JobQueue>	owner;
	std::shared_ptr<Job>				job;
};

class TimerItem
{
public:
	bool operator<(const TimerItem& other) const
	{
		return executeTick < other.executeTick;
	}

	uint64 executeTick = 0;
	JobData* jobData = nullptr;
};

class JobTimer : public RefSingleton<JobTimer>
{
public:
	void			Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job);
	void			Distribute(uint64 now);
	void			Clear();

private:
	USE_LOCK;
	std::priority_queue<TimerItem>	_items;
	std::atomic_bool				_distributing = false;
};

