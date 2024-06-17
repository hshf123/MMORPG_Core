#include "pch.h"
#include "DBLoadBalancer.h"

bool DBLoadBalancer::Init(const std::string& connectionString, const int32& serviceCount)
{
	if (connectionString.empty() || serviceCount <= 0)
		return false;

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
