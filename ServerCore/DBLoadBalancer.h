#pragma once
#include "DBService.h"

constexpr uint64 DBRECONNECTTIME = UINT64_C(5000);	// DB 재연결 시간

class DBLoadBalancer
{
public:
	bool Init(const std::string& connectionString, const int32& serviceCount);
	bool Push(const int32& workId, const uint16& protocolId, std::shared_ptr<DBData> data, DBHandler& handler);
	bool Push(std::shared_ptr<DBData> data, DBHandler& handler);

	void Launch();

private:
	std::string _connectionString = "";
	DBService*	_serviceList = nullptr;	// 배열로 사용
	int32		_serviceCount = 0;
};

