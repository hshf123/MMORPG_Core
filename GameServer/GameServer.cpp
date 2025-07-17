#include "pch.h"
#include "GameServer.h"
#include "Socket.h"
#include "ClientPacketHandler.h"
#include "GameDBLoadBalancer.h"
#include "GameDBHandler.h"
#include "Service.h"
#include "ClientSession.h"
#include "ThreadManager.h"
#include "ZoneManager.h"

void TimerJobQueue::UpdateTime()
{
	VIEW_WRITE_INFO("CPU ({:.2f}), MEOMORY ({:.2f})MB", Monitor::GetInstance().GetCPUUsage(), Monitor::GetInstance().GetMemoryUsage_MB());
	Monitor::GetInstance().PrintServerCounting();
	DoTimer(TimeUtils::OneMin, &TimerJobQueue::UpdateTime);
}

void TimerJobQueue::UpdateActor()
{
	DoAsyncToss([]()
		{
			ZoneManager::GetInstance().ActorTest();
		});
	DoTimer(TimeUtils::OneSec * 3, &TimerJobQueue::UpdateActor);
}

bool GameServer::Init()
{
	ThreadManager::GetInstance();	// 메인스레드 ID 생성 위해
	if (_ReadConfig() == false)
		return false;

	Socket::Init();
	LogManager::GetInstance().Initialize(ServerConfig::GetInstance().GetProcessName());
	GameDBHandler::GetInstance().Init();
	ClientPacketHandler::GetInstance().Init();

	if (_InitGameDB() == false)
	{
		VIEW_ERROR("GameDB Init Fail");
		return false;
	}
	if (_InitClientService() == false)
	{
		VIEW_ERROR("ClientService Init Fail");
		return false;
	}

	return true;
}

bool GameServer::Update()
{
	LogManager::GetInstance().Launch();
	GameDBLoadBalancer::Balancer->Launch();
	std::shared_ptr<DBData> data = PoolAlloc<DBData>();
	data->ProtocolID = EDBProtocol::SGDB_ServerStart;
	GameDBLoadBalancer::Balancer->Push(std::move(data));
	TimeUtils::WaitInit();

	std::shared_ptr<TimerJobQueue> jobQueue = std::make_shared<TimerJobQueue>();
	jobQueue->UpdateTime();

	while (true)
	{
		LEndTickCount = TimeUtils::GetTick64() + 64;
		ThreadManager::GetInstance().DistributeReservedJobs();
		ThreadManager::GetInstance().DoGlobalQueueWork();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return true;
}

bool GameServer::_ReadConfig()
{
	if (ServerConfig::GetInstance().ReadConfig("Config.json") == false)
		return false;

	xreserve<Job>(100'000, nullptr);
	xreserve<DBQueueData>(100'000, 0, nullptr, GameDBHandler::GetInstance());
	return true;
}

bool GameServer::_InitGameDB()
{
	if (GameDBLoadBalancer::Balancer->Init(
		ServerConfig::GetInstance().GetDBConnectionString()
		, ServerConfig::GetInstance().GetGameDBThreadCount()) == false)
		return false;
	//if (_redisIP.empty())
	//	return true;
	//if (GameDBLoadBalancer::Balancer->RedisInit(_redisIP, _redisPort) == false)
	//	return false;
	return true;
}

bool GameServer::_InitClientService()
{
	std::shared_ptr<ServerService> clientService = PoolAlloc<ServerService>(
		NetAddress(L"0.0.0.0", 9999),
		PoolAlloc<IocpCore>(),
		PoolAlloc<ClientSession>,
		100);
	ASSERT_CRASH(clientService->Start());
	if (ServerConfig::GetInstance().GetUseRIO())
	{
		for (int32 i = 0; i < ServerConfig::GetInstance().GetClientServiceCount(); i++)
		{
			if (clientService->CreateRIOCQ() == false)
				return false;
		}
	}
	_InitWorkerThread(clientService);
	return true;
}

void GameServer::_InitWorkerThread(std::shared_ptr<ServerService> service)
{
	for (int32 i = 0; i < ServerConfig::GetInstance().GetClientServiceCount(); i++)
	{
		ThreadManager::GetInstance().Launch([service]()
			{
				while (true)
				{
					LEndTickCount = TimeUtils::GetTick64() + 64;
					service->GetIocpCore()->Dispatch(10);
					ThreadManager::GetInstance().DistributeReservedJobs();
					ThreadManager::GetInstance().DoGlobalQueueWork();

					//std::shared_ptr<TimerJobQueue> jobQueue = std::make_shared<TimerJobQueue>();
					//jobQueue->UpdateActor();

					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
	}
}
