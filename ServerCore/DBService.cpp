#include "pch.h"
#include "DBService.h"

bool DBService::Connect(const std::string& connectionString)
{
	try
	{
		_connectionString = connectionString;
		_session = new Poco::Data::Session(Poco::Data::ODBC::Connector::KEY, _connectionString, 5);
	}
	catch (Poco::Data::ConnectionFailedException& ex)
	{
		std::cout << std::format("[DB: {}] Connect Failed Cause \n{}", _connectionString, StrUtils::ToString(ex.displayText().c_str())) << std::endl;
		return false;
	}

	VIEW_INFO("[DB: {}] Connect Success!!", _connectionString);
	return true;
}

bool DBService::RedisConnect(const std::string& ip, int32 port)
{
	_redisContext = redisConnect(ip.c_str(), port);
	if (_redisContext == nullptr || _redisContext->err)
	{
		std::cout << std::format("[Redis: {}/{}] Connect Failed Cause \n{}", _redisIP, _redisPort) << std::endl;
		return false;
	}
	return true;
}

