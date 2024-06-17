#include "pch.h"
#include "DBService.h"

bool DBService::Connect(const std::string& connectionString)
{
	try
	{
		Poco::Data::Session* pSession = new Poco::Data::Session(Poco::Data::ODBC::Connector::KEY, _connectionString, 5);
	}
	catch (Poco::Data::ConnectionFailedException& ex)
	{
		VIEW_WRITE_ERROR("[DB: {}] Connect Failed Cause \n{}", _connectionString, ex.displayText());
		return false;
	}

	return true;
}
