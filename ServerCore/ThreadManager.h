#pragma once
#include "Singleton.h"

class ThreadManager : public RefSingleton<ThreadManager>
{
public:
	ThreadManager();
	~ThreadManager();

	/// <summary>
	/// 스레드 생성 후 함수 실행
	/// </summary>
	/// <param name="fn">함수 포인터</param>
	void Launch(std::function<void(void)> fn);

	static void InitTLS();

	/// <summary>
	/// 글로벌 큐 작업 실행
	/// </summary>
	static void DoGlobalQueueWork();
	/// <summary>
	/// 예약 일감 작업 Push
	/// </summary>
	static void DistributeReservedJobs();

private:
	USE_LOCK;
	std::vector<std::jthread> _threads;
};

