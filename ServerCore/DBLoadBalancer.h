#pragma once
#include "DBService.h"

class DBLoadBalancer
{
public:
	bool Init(const std::string& connectionString, const int32& serviceCount);
	bool Push(const int32& workId, const uint16& protocolId, std::shared_ptr<DBData> data, DBHandler& handler);

private:
	DBService*	_serviceList = nullptr;	// �迭�� ���
	int32		_serviceCount = 0;
};
