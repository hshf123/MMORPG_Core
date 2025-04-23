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

bool DBService::Push(const uint16& protocolId, std::shared_ptr<DBData> data, DBHandler& handler)
{
	if (data == nullptr)
		return false;

	_queueCount.fetch_add(1);
	_dbQueue.Push(xnew<DBQueueData>(protocolId, data, handler));
	return true;
}

void DBService::Execute()
{
	while (true)
	{
		if (_queueCount == 0)
			return;

		std::vector<DBQueueData*> jobs;
		_dbQueue.PopAll(OUT jobs);

		const int32 queueCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < queueCount; i++)
		{
			jobs[i]->handler.HandleData(jobs[i]->ProtocolId, jobs[i]->data);
			jobs[i]->data = nullptr;
			xdelete(jobs[i]);
		}

		// 남은 일감이 0개라면 종료
		if (_queueCount.fetch_sub(queueCount) == queueCount)
			return;
	}
}
