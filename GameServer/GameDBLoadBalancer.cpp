#include "pch.h"
#include "GameDBLoadBalancer.h"
#include "GameDBHandler.h"

std::shared_ptr<GameDBLoadBalancer> GameDBLoadBalancer::Balancer = PoolAlloc<GameDBLoadBalancer>();

bool GameDBLoadBalancer::Push(int32 workId, uint16 protocolId, std::shared_ptr<DBData> data)
{
	if (data != nullptr)
	{
		data->WorkID = workId;
		data->ProtocolID = protocolId;
	}
	return DBLoadBalancer::Push(workId, protocolId, data, GameDBHandler::GetInstance());
}

bool GameDBLoadBalancer::Push(std::shared_ptr<DBData> data)
{
	if (data != nullptr && data->ProtocolID == 0)
		VIEW_WRITE_ERROR("DBData ProtocolID is zero!!");
	return DBLoadBalancer::Push(data, GameDBHandler::GetInstance());
}
