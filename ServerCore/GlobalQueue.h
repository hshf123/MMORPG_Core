#pragma once
#include "Singleton.h"
#include "LockQueue.h"

class JobQueue;

/// <summary>
/// DB�۾� ����, ���� �� ��Ʈ��ũ ��Ŷ �۾� ť
/// �� �����忡 �ʹ� ���� �۾��� ���� ��츦 ���� ť, DB�� �ε�뷱���� ����
/// </summary>
class GlobalQueue : public RefSingleton<GlobalQueue>
{
public:
	void									Push(std::shared_ptr<JobQueue> jobQueue);
	std::shared_ptr<JobQueue>				Pop();

private:
	LockQueue<std::shared_ptr<JobQueue>> _jobQueues;
};
