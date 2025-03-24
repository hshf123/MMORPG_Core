#include "pch.h"
#include "LogManager.h"
#include "Service.h"
#include "IOCP.h"
#include "ClientSession.h"
#include "ThreadManager.h"
#include "GameDBLoadBalancer.h"
#include "GameDBHandler.h"
#include "ClientPacketHandler.h"
#include "JobQueue.h"
#include "Monitor.h"

//rapidjson 사용예제
//const std::string json = "{\"project\":\"rapidjson\",\"stars\":10}";
//rapidjson::Document d;
//d.Parse(json.c_str());
//rapidjson::Value& s = d["stars"];
//s.SetInt(s.GetInt() + 1);
//rapidjson::StringBuffer buffer;
//rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//d.Accept(writer);

class TimerJobQueue : public JobQueue
{
public:
	void UpdateTime()
	{
		VIEW_INFO("Server is running...");
		VIEW_INFO("CPU ({:.2f}), MEOMORY ({:.2f})MB", Monitor::GetInstance().GetCPUUsage(), Monitor::GetInstance().GetMemoryUsage_MB());
		DoTimer(TimeUtils::OneMin / 2, &TimerJobQueue::UpdateTime);
	}
};

uint32 GetThreadCount()
{
	std::thread t;
	return t.hardware_concurrency() / 4;
}

int main()
{
	Socket::Init();
	LogManager::GetInstance().Initialize("GameServer");
	GameDBHandler::GetInstance().Init();
	ClientPacketHandler::GetInstance().Init();
	GameDBLoadBalancer::Balancer->Init("Driver={ODBC Driver 17 for SQL Server};Server=(LocalDB)\\MSSQLLocalDB;Database=Game;Trusted_Connection=Yes;", GetThreadCount());

	LogManager::GetInstance().Launch();
	GameDBLoadBalancer::Balancer->Launch();
	std::shared_ptr<DBData> data =  PoolAlloc<DBData>();
	data->ProtocolID = EDBProtocol::SGDB_ServerStart;
	GameDBLoadBalancer::Balancer->Push(data);
	TimeUtils::WaitInit();

	std::shared_ptr<ServerService> clientService = PoolAlloc<ServerService>(
		NetAddress(L"127.0.0.1", 9999),
		PoolAlloc<IocpCore>(),
		PoolAlloc<ClientSession>,
		10);
	ASSERT_CRASH(clientService->Start());
	for (uint32 i = UINT32_C(0); i < GetThreadCount(); i++)
	{
		if (clientService->CreateRIOCQ() == false)
			return 0;
	}
	for (uint32 i = UINT32_C(0); i < GetThreadCount(); i++)
	{
		ThreadManager::GetInstance().Launch([&]()
			{
				while (true)
				{
					clientService->GetIocpCore()->Dispatch(10);
					LEndTickCount = TimeUtils::GetTick64() + 64;
					ThreadManager::GetInstance().DistributeReservedJobs();
					ThreadManager::GetInstance().DoGlobalQueueWork();

					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
	}

#ifdef DEV_TEST
	// 테스트용
	std::shared_ptr<TimerJobQueue> jobQueue = std::make_shared<TimerJobQueue>();
	jobQueue->UpdateTime();
#endif
	while (true)
	{
		LEndTickCount = TimeUtils::GetTick64() + 64;
		ThreadManager::GetInstance().DistributeReservedJobs();
		ThreadManager::GetInstance().DoGlobalQueueWork();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}