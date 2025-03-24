#pragma once
#include "JobQueue.h"

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
	bool _ReadConfig();
	bool _InitGameDB();
	bool _InitClientService();
	void _InitWorkerThread(std::shared_ptr<class ServerService> service);

private:
	std::string _processName = {};
	std::string _gameDBConnectionString = {};
	int32 _gameDBThreadCount = 0;
	int32 _clientServiceThreadCount = 0;
};

