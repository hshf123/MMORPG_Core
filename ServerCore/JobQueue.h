#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"

/// <summary>
/// 기본적으로 상속 받아서 사용.
/// 큐에 넣어서 시간이 딜레이 되는 것을 막고 싶을 때는 락 걸로 바로 호출하면 됨
/// 꼭 상속받았다고 해서 무조건 사용해야하는 것은 아님.
/// </summary>
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
	/// <summary>
	/// 작업 큐에 일감 Push
	/// 멀티스레드 환경에서도 큐의 일감을 처리할 수 있는건 한 스레드 뿐
	/// 이미 다른 스레드가 처리 중이라면 GlobalQueue로 넘어감
	/// </summary>
	/// <param name="job">일감</param>
	/// <param name="pushOnly">true : 일감을 처리하는 스레드가 없어도 무조건 GlobalQueue에 삽입</param>
	void Push(std::shared_ptr<Job> job, bool pushOnly = false);
	void					Execute();

protected:
	LockQueue<std::shared_ptr<Job>>		_jobs;
	std::atomic_int32_t					_jobCount = 0;
};

