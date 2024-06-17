#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(CallbackType&& callback)
	{
		Push(PoolAlloc<Job>(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(PoolAlloc<Job>(owner, memFunc, std::forward<Args>(args)...));
	}

	void DoTimer(uint64 tickAfter, CallbackType&& callback)
	{
		std::shared_ptr<Job> job = PoolAlloc<Job>(std::move(callback));
		JobTimer::GetInstance().Reserve(tickAfter, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(uint64 tickAfter, Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		std::shared_ptr<Job> job = PoolAlloc<Job>(owner, memFunc, std::forward<Args>(args)...);
		JobTimer::GetInstance().Reserve(tickAfter, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoUpdate(uint64 tickAfter, Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		std::shared_ptr<Job> job = PoolAlloc<Job>(owner, memFunc, std::forward<Args>(args)...);
		JobTimer::GetInstance().UpdateReserve(tickAfter, shared_from_this(), job);
	}

	void					ClearJobs() { _jobs.Clear(); }

public:
	void Push(std::shared_ptr<Job> job, bool pushOnly = false);
	void					Execute();

protected:
	LockQueue<std::shared_ptr<Job>>		_jobs;
	std::atomic_int32_t					_jobCount = 0;
};

