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
		_serviceList[i].Connect(connectionString);
	_serviceCount = serviceCount;
	return true;
}

bool DBLoadBalancer::Push(const int32& workId, const uint16& protocolId, std::shared_ptr<DBData> data, DBHandler& handler)
{
	if (_serviceList == nullptr)
		return false;

	_serviceList[workId % _serviceCount].Push(protocolId, data, handler);
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
		ThreadManager::GetInstance().Launch([&, i]()
			{
				while (true)
				{
					if (_serviceList[i].GetDBSession() != nullptr && _serviceList[i].GetDBSession()->isConnected() == false)
					{
						_serviceList[i].Connect(_connectionString);
						continue;
					}

					_serviceList[i].Execute();
				}
			});
	}
}
