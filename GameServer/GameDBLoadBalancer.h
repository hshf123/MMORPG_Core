#pragma once
#include "DBLoadBalancer.h"

class GameDBLoadBalancer : public DBLoadBalancer
{
public:
	bool Push(const int32& workId, const uint16& protocolId, std::shared_ptr<DBData> data);
	bool Push(std::shared_ptr<DBData> data);
};
