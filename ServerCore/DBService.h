#pragma once
#include "LockQueue.h"
#include "TimeUtils.h"

#include <Poco/Data/RecordSet.h>
#include <Poco/Data/ODBC/ODBCException.h>
#include <Poco/Data/ODBC/ODBCStatementImpl.h>

#pragma comment (lib, "iphlpapi.lib")	// GetAdaptersInfo()
using namespace Poco::Data::Keywords;		// in() out()

class DBService
{
public:
	/// <summary>
	/// DB 연결 직접호출 하지 않고 로드밸런서를 통해 호출
	/// </summary>
	/// <param name="connectionString"></param>
	/// <returns></returns>
	bool Connect(const std::string& connectionString);
	/// <summary>
	/// DB 연결 직접호출 하지 않고 로드밸런서를 통해 호출
	/// </summary>
	/// <param name="ip"></param>
	/// <param name="port"></param>
	/// <returns></returns>
	bool RedisConnect(const std::string& ip, int32 port);

	Poco::Data::Session* GetDBSession() { return _session; }

	template<class Key, class Value>
	bool SetRedisValue(Key key, Value value);
	template<class Key, class Value>
	bool GetRedisValue(Key key, OUT Value& value);

private:
	std::string _connectionString;
	Poco::Data::Session* _session = nullptr;

	std::string _redisIP;
	int32 _redisPort = 0;
	redisContext* _redisContext = nullptr;
};
