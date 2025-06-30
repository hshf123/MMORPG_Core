#pragma once
#include "Singleton.h"

class ServerConfig : public RefSingleton<ServerConfig>
{
public:
	const bool ReadConfig(const std::string& config);

	std::string GetProcessName() const;
	std::string GetDBConnectionString() const;
	std::string GetRedisIP() const;
	int32 GetRedisPort() const;
	int32 GetGameDBThreadCount() const;
	int32 GetClientServiceCount() const;
	bool GetUseRIO() const;

private:
	std::string _processName = {};
	std::string _gameDBConnectionString = {};
	std::string _redisIP = {};
	int32 _redisPort = 0;
	int32 _gameDBThreadCount = 0;
	int32 _clientServiceThreadCount = 0;
	bool _useRIO = false;
};

