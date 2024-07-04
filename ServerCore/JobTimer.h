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

/// <summary>
/// 당장 실행이 아닌 일정 시간 뒤에 처리할 작업을 처리하기 위함
/// </summary>
class JobTimer : public RefSingleton<JobTimer>
{
public:
	/// <summary>
	/// 예약 일감 Push
	/// </summary>
	/// <param name="tickAfter">처리 되어야 할 시간</param>
	/// <param name="owner">JobQueue를 상속받은 smart pointer</param>
	/// <param name="job"></param>
	void			Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job);	
	/// <summary>
	/// 예약 일감 실행, 예약된 일감이 실행된다고 해도 Queue에 넣는 것이기 때문에 즉시 실행 되는 것은
	/// 아니기 때문에 서버가 느려지면 작업이 밀릴 수 있음
	/// 정밀한 시간 체크를 하고 싶다면 다른 방법을 마련해야함.
	/// </summary>
	/// <param name="now">현재 시간</param>
	void			Distribute(uint64 now);
	void			Clear();

private:
	USE_LOCK;
	std::priority_queue<TimerItem>	_items;
	std::atomic_bool				_distributing = false;
};
