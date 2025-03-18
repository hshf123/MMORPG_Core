#pragma once
#include "DBLoadBalancer.h"

class GameDBLoadBalancer : public DBLoadBalancer
{
public:
	static std::shared_ptr<GameDBLoadBalancer> Balancer;

public:
	bool Push(int32 workId, uint16 protocolId, std::shared_ptr<DBData> data);
	bool Push(std::shared_ptr<DBData> data);
};
