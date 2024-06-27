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
	/// <summary>
	/// DB 연결 직접호출 하지 않고 로드밸런서를 통해 호출
	/// </summary>
	/// <param name="connectionString"></param>
	/// <returns></returns>
	bool Connect(const std::string& connectionString);
	/// <summary>
	/// DB 작업 Push 직접호출 하지 않고 로드밸런서를 통해 호출
	/// </summary>
	/// <param name="protocolId"></param>
	/// <param name="data"></param>
	/// <param name="handler"></param>
	/// <returns></returns>
	bool Push(const uint16& protocolId, std::shared_ptr<DBData> data, DBHandler& handler);
	/// <summary>
	/// 해당 스레드에 배정된 DB 작업을 처리
	/// </summary>
	void Execute();

	Poco::Data::Session* GetDBSession() { return _session; }

private:
	std::string _connectionString = "";
	Poco::Data::Session* _session = nullptr;

	std::atomic_int32_t _queueCount = 0;
	LockQueue<std::shared_ptr<DBQueueData>> _dbQueue;	// DB 작업 큐
};
