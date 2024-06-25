#include "pch.h"
#include "GameDBLoadBalancer.h"
#include "GameDBHandler.h"

bool GameDBLoadBalancer::Push(const int32& workId, const uint16& protocolId, std::shared_ptr<DBData> data)
{
	return DBLoadBalancer::Push(workId, protocolId, data, GameDBHandler::GetInstance());
}

bool GameDBLoadBalancer::Push(std::shared_ptr<DBData> data)
{
	return DBLoadBalancer::Push(data, GameDBHandler::GetInstance());
}
