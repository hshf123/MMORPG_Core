#pragma once
#include "JobQueue.h"

class TimerJobQueue : public JobQueue
{
public:
	void UpdateTime();
	void UpdateActor();
};

class GameServer
{
public:
	bool Init();
	bool Update();

private:
	bool _ReadConfig();
	bool _InitGameDB();
	bool _InitClientService();
	void _InitWorkerThread(std::shared_ptr<class ServerService> service);
};

