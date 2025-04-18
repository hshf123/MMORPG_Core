#include "pch.h"
#include "DBLoadBalancer.h"
#include "ThreadManager.h"

bool DBLoadBalancer::Init(const std::string& connectionString, const int32& serviceCount)
{
	if (connectionString.empty() || serviceCount <= 0)
		return false;

	_connectionString = connectionString;
	Poco::Data::ODBC::Connector::registerConnector();
	_serviceList = new DBService[serviceCount]();
	for (int32 i = 0; i < serviceCount; i++)
	{
		if (_serviceList[i].Connect(connectionString) == false)
			return false;
	}
	_serviceCount = serviceCount;
	return true;
}

bool DBLoadBalancer::Init(const std::string& driver, const std::string& id, const std::string& pwd, const std::string& db, const std::string& host, const std::string& port, const int32& serviceCount)
{
	std::string connectionString = std::format("DRIVER={};UID={};PWD={};DATABASE={};SERVER={},{};", driver, id, pwd, db, host, port);
	return Init(connectionString, serviceCount);
}

bool DBLoadBalancer::Push(int32 workId, uint16 protocolId, std::shared_ptr<DBData> data, DBHandler& handler)
{
	if (_serviceList == nullptr)
		return false;
	GetDBService(workId).Push(protocolId, data, handler);
	return true;
}

bool DBLoadBalancer::Push(std::shared_ptr<DBData> data, DBHandler& handler)
{
	if (data == nullptr)
		return false;

	return Push(data->WorkID, data->ProtocolID, data, handler);
}

void DBLoadBalancer::Launch()
{
	for (int32 i = 0; i < _serviceCount; i++)
	{
		DBService& service = _serviceList[i];
		ThreadManager::GetInstance().Launch([&]()
			{
				while (true)
				{
					if (service.GetDBSession() == nullptr)
						continue;

					if (service.GetDBSession()->isConnected() == false && LEndTickCount < TimeUtils::GetTick64())
					{
						service.Connect(_connectionString);
						LEndTickCount = TimeUtils::GetTick64() + DBRECONNECTTIME;
						continue;
					}

					service.Execute();

					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
	}
}

DBService& DBLoadBalancer::GetDBService(int32 workId)
{
	return _serviceList[workId % _serviceCount];
}
