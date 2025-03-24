#include "pch.h"
#include "GameServer.h"
#include "Socket.h"
#include "ClientPacketHandler.h"
#include "GameDBLoadBalancer.h"
#include "GameDBHandler.h"
#include "Service.h"
#include "ClientSession.h"
#include "ThreadManager.h"

void TimerJobQueue::UpdateTime()
{
	VIEW_WRITE_INFO("CPU ({:.2f}), MEOMORY ({:.2f})MB", Monitor::GetInstance().GetCPUUsage(), Monitor::GetInstance().GetMemoryUsage_MB());
	DoTimer(TimeUtils::OneMin / 2, &TimerJobQueue::UpdateTime);
}

bool GameServer::Init()
{
	Socket::Init();
	LogManager::GetInstance().Initialize("GameServer");
	GameDBHandler::GetInstance().Init();
	ClientPacketHandler::GetInstance().Init();

	if (_InitGameDB() == false)
		return false;
	if (_InitClientService() == false)
		return false;

	return true;
}

bool GameServer::Update()
{
	LogManager::GetInstance().Launch();
	GameDBLoadBalancer::Balancer->Launch();
	std::shared_ptr<DBData> data = PoolAlloc<DBData>();
	data->ProtocolID = EDBProtocol::SGDB_ServerStart;
	GameDBLoadBalancer::Balancer->Push(data);
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

bool GameServer::_InitGameDB()
{
	return GameDBLoadBalancer::Balancer->Init(
		"Driver={ODBC Driver 17 for SQL Server};Server=(LocalDB)\\MSSQLLocalDB;Database=Game;Trusted_Connection=Yes;"
		, THREAD_COUNT);
}

bool GameServer::_InitClientService()
{
	std::shared_ptr<ServerService> clientService = PoolAlloc<ServerService>(
		NetAddress(L"127.0.0.1", 9999),
		PoolAlloc<IocpCore>(),
		PoolAlloc<ClientSession>,
		100);
	ASSERT_CRASH(clientService->Start());
	for (uint32 i = UINT32_C(0); i < THREAD_COUNT; i++)
	{
		if (clientService->CreateRIOCQ() == false)
			return false;
	}

	_InitWorkerThread(clientService);
	return true;
}

void GameServer::_InitWorkerThread(std::shared_ptr<ServerService> service)
{
	for (uint32 i = UINT32_C(0); i < THREAD_COUNT; i++)
	{
		ThreadManager::GetInstance().Launch([service]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch(10);
					LEndTickCount = TimeUtils::GetTick64() + 64;
					ThreadManager::GetInstance().DistributeReservedJobs();
					ThreadManager::GetInstance().DoGlobalQueueWork();

					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
	}
}
