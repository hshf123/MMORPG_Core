#include "pch.h"
#include "GlobalQueue.h"

void GlobalQueue::Push(std::shared_ptr<JobQueue> jobQueue)
{
	_jobQueues.Push(jobQueue);
}

std::shared_ptr<JobQueue> GlobalQueue::Pop()
{
	return _jobQueues.Pop();
}

void GlobalQueue::PushDBData(std::shared_ptr<DBData> data)
{
	_dbDataQueues.Push(data);
}

void GlobalQueue::PopAllDBData(OUT std::vector<std::shared_ptr<DBData>>& datas)
{
	return _dbDataQueues.PopAll(OUT datas);
}
