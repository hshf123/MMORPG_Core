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
		VIEW_WRITE_ERROR("[DB: {}] Connect Failed Cause \n{}", _connectionString, ex.displayText());
		return false;
	}

	return true;
}

bool DBService::Push(const uint16& protocolId, std::shared_ptr<DBData> data, DBHandler& handler)
{
	if (data == nullptr)
		return false;

	_dbQueue.Push(PoolAlloc<DBQueueData>(protocolId, data, handler));
	return true;
}

void DBService::Execute()
{
	while (true)
	{
		std::vector<std::shared_ptr<DBQueueData>> jobs;
		_dbQueue.PopAll(OUT jobs);

		const int32 queueCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < queueCount; i++)
			jobs[i]->handler.HandleData(jobs[i]->ProtocolId, jobs[i]->data, this);

		// 남은 일감이 0개라면 종료
		if (_queueCount.fetch_sub(queueCount) == queueCount)
			return;
	}
}
