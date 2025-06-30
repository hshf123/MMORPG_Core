#include "pch.h"
#include "ServerConfig.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

const bool ServerConfig::ReadConfig(const std::string& config)
{
	if (fs::exists(config) == false)
	{
		VIEW_ERROR("Not Found Config");
		return false;
	}
	std::ifstream file(config, std::ios::binary);
	if (file.fail())
	{
		VIEW_ERROR("Config Read Fail");
		return false;
	}
	std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	rapidjson::Document doc;
	doc.Parse(json.c_str());

	rapidjson::Value& mainValue = doc["Main"];
	_processName = mainValue["ProcessName"].GetString();
	VIEW_INFO("{}", _processName);
	_clientServiceThreadCount = mainValue["ThreadCount"].GetInt();
	VIEW_INFO("ClientService ThreadCount : {}", _clientServiceThreadCount);
	_useRIO = mainValue["UseRIO"].GetBool();
	VIEW_INFO("Network I/O : {}", _useRIO ? "RIO" : "IOCP");

	if (doc.HasMember("GameDB"))
	{
		rapidjson::Value& gdbValue = doc["GameDB"];

		if (gdbValue.HasMember("ConnectionString"))
		{
			_gameDBConnectionString = gdbValue["ConnectionString"].GetString();
		}
		else
		{
			_gameDBConnectionString =
				std::format("DRIVER=SQL SERVER;UID={};PWD={};DATABASE={};SERVER={},{};",
					gdbValue["ID"].GetString(),
					gdbValue["PWD"].GetString(),
					gdbValue["DB"].GetString(),
					gdbValue["HOST"].GetString(),
					gdbValue["PORT"].GetString());
		}
		_gameDBThreadCount = gdbValue["ThreadCount"].GetInt();
		VIEW_INFO("GameDB Connection Count : {}", _gameDBThreadCount);
	}

	if (doc.HasMember("Redis"))
	{
		rapidjson::Value& redisValue = doc["Redis"];
		_redisIP = redisValue["IP"].GetString();
		_redisPort = redisValue["Port"].GetInt();
	}

	//{
	//	// 쓰기 테스트
	//	const std::string json = "{\"project\":\"rapidjson\",\"stars\":10}";
	//	rapidjson::Document d;
	//	d.Parse(json.c_str());
	//	rapidjson::Value& s = d["stars"];
	//	s.SetInt(s.GetInt() + 1);
	//	rapidjson::StringBuffer buffer;
	//	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	//	d.Accept(writer);
	//	std::string str = buffer.GetString();
	//}

	return true;
}

std::string ServerConfig::GetProcessName() const
{
	return _processName;
}

std::string ServerConfig::GetDBConnectionString() const
{
	return _gameDBConnectionString;
}

std::string ServerConfig::GetRedisIP() const
{
	return _redisIP;
}

int32 ServerConfig::GetRedisPort() const
{
	return _redisPort;
}

int32 ServerConfig::GetGameDBThreadCount() const
{
	return _gameDBThreadCount;
}

int32 ServerConfig::GetClientServiceCount() const
{
	return _clientServiceThreadCount;
}

bool ServerConfig::GetUseRIO() const
{
	return _useRIO;
}
