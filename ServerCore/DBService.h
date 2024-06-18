#pragma once
#include "LockQueue.h"
#include "DBHandler.h"
#include "TimeUtils.h"

#include <Poco/Data/RecordSet.h>
#include <Poco/Data/ODBC/ODBCException.h>
#include <Poco/Data/ODBC/ODBCStatementImpl.h>

#pragma comment (lib, "iphlpapi.lib")	// GetAdaptersInfo()
using namespace Poco::Data::Keywords;		// in() out()

struct DBQueueData 
{
	uint16 ProtocolId = 0;
	std::shared_ptr<DBData> data = nullptr;
	DBHandler& handler;
};

class DBService
{
public:
	bool Connect(const std::string& connectionString);
	bool Push(const uint16& protocolId, std::shared_ptr<DBData> data, DBHandler& handler);
	void Execute();

private:
	std::string _connectionString = "";
	Poco::Data::Session* _session = nullptr;

	std::atomic_int32_t _queueCount = 0;
	LockQueue<std::shared_ptr<DBQueueData>> _dbQueue;
};

