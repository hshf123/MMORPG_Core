#pragma once
#include "JobQueue.h"

#define THREAD_COUNT 4

class TimerJobQueue : public JobQueue
{
public:
	void UpdateTime();
};

class GameServer
{
public:
	bool Init();
	bool Update();

private:
	bool _InitGameDB();
	bool _InitClientService();
	void _InitWorkerThread(std::shared_ptr<class ServerService> service);
};

