#pragma once
#include "Singleton.h"
#include "LockQueue.h"

class JobQueue;
class DBData;

/// <summary>
/// DB작업 제외, 로직 및 네트워크 패킷 작업 큐
/// 한 스레드에 너무 많은 작업이 몰릴 경우를 위한 큐, DB는 로드밸런서가 수행
/// </summary>
class GlobalQueue : public RefSingleton<GlobalQueue>
{
public:
	void									Push(std::shared_ptr<JobQueue> jobQueue);
	std::shared_ptr<JobQueue>				Pop();

	void									PushDBData(std::shared_ptr<DBData> data);
	void									PopDBData(OUT std::vector<std::shared_ptr<DBData>>& items);

private:
	LockQueue<std::shared_ptr<JobQueue>> _jobQueues;
	LockQueue<std::shared_ptr<DBData>> _dbDataQueues;
};
