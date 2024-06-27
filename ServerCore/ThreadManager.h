#pragma once
#include "Singleton.h"

class ThreadManager : public RefSingleton<ThreadManager>
{
public:
	ThreadManager();
	~ThreadManager();

	/// <summary>
	/// ������ ���� �� �Լ� ����
	/// </summary>
	/// <param name="fn">�Լ� ������</param>
	void Launch(std::function<void(void)> fn);

	static void InitTLS();

	/// <summary>
	/// �۷ι� ť �۾� ����
	/// </summary>
	static void DoGlobalQueueWork();
	/// <summary>
	/// ���� �ϰ� �۾� Push
	/// </summary>
	static void DistributeReservedJobs();

private:
	USE_LOCK;
	std::vector<std::jthread> _threads;
};

