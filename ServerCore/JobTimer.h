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
/// ���� ������ �ƴ� ���� �ð� �ڿ� ó���� �۾��� ó���ϱ� ����
/// </summary>
class JobTimer : public RefSingleton<JobTimer>
{
public:
	/// <summary>
	/// ���� �ϰ� Push
	/// </summary>
	/// <param name="tickAfter">ó�� �Ǿ�� �� �ð�</param>
	/// <param name="owner">JobQueue�� ��ӹ��� smart pointer</param>
	/// <param name="job"></param>
	void			Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job);	
	/// <summary>
	/// ���� �ϰ� ����, ����� �ϰ��� ����ȴٰ� �ص� Queue�� �ִ� ���̱� ������ ��� ���� �Ǵ� ����
	/// �ƴϱ� ������ ������ �������� �۾��� �и� �� ����
	/// ������ �ð� üũ�� �ϰ� �ʹٸ� �ٸ� ����� �����ؾ���.
	/// </summary>
	/// <param name="now">���� �ð�</param>
	void			Distribute(uint64 now);
	void			Clear();

private:
	USE_LOCK;
	std::priority_queue<TimerItem>	_items;
	std::atomic_bool				_distributing = false;
};
