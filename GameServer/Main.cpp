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
#include "pdh.h"
#pragma comment(lib, "pdh.lib")

/*
	1. 서버 뜰 때 초기화
	2. DB 연결 데이터 로딩
	3. 리슨, 타 서버 커넥트(있으면)
	4. 데이터 로딩 끝난 후 초기화
*/

class TimerJobQueue : public JobQueue
{
public:
	void UpdateTime()
	{
		VIEW_INFO("Server is running...");
		DoTimer(TimeUtils::OneHour, &TimerJobQueue::UpdateTime);
	}
};

uint32 GetThreadCount()
{
#ifdef DEV_TEST
	return 8;
#else
	std::thread t;
	return t.hardware_concurrency();
#endif
}

int main()
{
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;
	::PdhOpenQuery(NULL, NULL, &cpuQuery);
	::PdhAddCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
	::PdhCollectQueryData(cpuQuery);

	Socket::Init();
	LogManager::GetInstance().Initialize("GameServer");
	GameDBHandler::GetInstance().Init();
	ClientPacketHandler::GetInstance().Init();
#ifdef DEV_TEST
	GameDBLoadBalancer::Balancer->Init("Driver={ODBC Driver 17 for SQL Server};Server=(LocalDB)\\MSSQLLocalDB;Database=Game;Trusted_Connection=Yes;", 1);
#else
	GameDBLoadBalancer::Balancer->Init("Driver={ODBC Driver 17 for SQL Server};Server=(LocalDB)\\MSSQLLocalDB;Database=Game;Trusted_Connection=Yes;", 8);
#endif
	
	//const std::string json = "{\"project\":\"rapidjson\",\"stars\":10}";
	//rapidjson::Document d;
	//d.Parse(json.c_str());
	//rapidjson::Value& s = d["stars"];
	//s.SetInt(s.GetInt() + 1);
	//rapidjson::StringBuffer buffer;
	//rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	//d.Accept(writer);

	LogManager::GetInstance().Launch();
	GameDBLoadBalancer::Balancer->Launch();
	std::shared_ptr<DBData> data =  PoolAlloc<DBData>();
	data->ProtocolID = EDBProtocol::SGDB_ServerStart;
	//GameDBLoadBalancer::Balancer->Push(data);

	std::shared_ptr<ServerService> clientService = PoolAlloc<ServerService>(
		NetAddress(L"0.0.0.0", 9999),
		PoolAlloc<IocpCore>(),
		PoolAlloc<ClientSession>,
		10);
	ASSERT_CRASH(clientService->Start());
	for (uint32 i = UINT32_C(0); i < GetThreadCount(); i++)
	{
		ThreadManager::GetInstance().Launch([&]()
			{
				clientService->CreateRIOCQ();
				while (true)
				{
					clientService->GetIocpCore()->Dispatch(10);
					LEndTickCount = TimeUtils::GetTick64() + 64;
					ThreadManager::DistributeReservedJobs();
					ThreadManager::DoGlobalQueueWork();

					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
	}

#ifdef DEV_TEST
	// 테스트용
	std::shared_ptr<TimerJobQueue> jobQueue = std::make_shared<TimerJobQueue>();
	jobQueue->UpdateTime();
	uint64 tick = TimeUtils::GetTick64();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (tick > TimeUtils::GetTick64())
			continue;
		PDH_FMT_COUNTERVALUE counterVal;
		::PdhCollectQueryData(cpuQuery);
		::PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		VIEW_INFO("CPU USAGE : {}", counterVal.doubleValue);
		tick = TimeUtils::GetTick64() + (TimeUtils::OneMin / 2);
	}
#else
	while (true)
	{
		LEndTickCount = TimeUtils::GetTick64() + 64;
		ThreadManager::DistributeReservedJobs();
		ThreadManager::DoGlobalQueueWork();
	}
#endif
	return 0;
}