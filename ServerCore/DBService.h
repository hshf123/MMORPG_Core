#pragma once

#ifndef POCO_STATIC
#define POCO_STATIC 1
#endif

#include <Poco/DateTime.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/ODBC/ODBCException.h>
#include <Poco/Data/ODBC/ODBCStatementImpl.h>

#pragma warning (pop)
#pragma comment (lib, "iphlpapi.lib")	// GetAdaptersInfo()
using namespace Poco::Data::Keywords;		// in() out()

class DBService
{
public:
	bool Connect(const std::string& connectionString);

private:
	std::string _connectionString = "";
};

