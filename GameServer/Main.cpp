#include "pch.h"
#include "LogManager.h"
#include "Service.h"
#include "IOCP.h"
#include "ClientSession.h"
#include "ThreadManager.h"
#include "GameDBLoadBalancer.h"
#include "GameDBHandler.h"
#include "ClientPacketHandler.h"

#include "pdh.h"
#pragma comment(lib, "pdh.lib")

/*
	1. 서버 뜰 때 초기화
	2. DB 연결 데이터 로딩
	3. 리슨, 타 서버 커넥트(있으면)
	4. 데이터 로딩 끝난 후 초기화
*/

uint32 GetThreadCount()
{
#ifdef DEV_TEST
	return 16;
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
	std::shared_ptr<GameDBLoadBalancer> tdbBalancer = PoolAlloc<GameDBLoadBalancer>();
#ifdef DEV_TEST
	tdbBalancer->Init("Driver={ODBC Driver 17 for SQL Server};Server=(LocalDB)\\MSSQLLocalDB;Database=Game;Trusted_Connection=Yes;", 1);
#else
	tdbBalancer->Init("Driver={ODBC Driver 17 for SQL Server};Server=(LocalDB)\\MSSQLLocalDB;Database=Game;Trusted_Connection=Yes;", 8);
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
	tdbBalancer->Launch();
	tdbBalancer->Push(PoolAlloc<DBData>(Protocol::EDBProtocol::SGDB_ServerStart, 0));

	std::shared_ptr<ServerService> clientService = PoolAlloc<ServerService>(
		NetAddress(L"127.0.0.1", 9999),
		PoolAlloc<IocpCore>(),
		PoolAlloc<ClientSession>,
		10);
	ASSERT_CRASH(clientService->Start());
	for (uint32 i = UINT32_C(0); i < GetThreadCount(); i++)
	{
		ThreadManager::GetInstance().Launch([&]()
			{
				while (true)
				{
					LEndTickCount = TimeUtils::GetTick64() + 64;
					clientService->GetIocpCore()->Dispatch(10);
#ifdef USE_RIO
					clientService->Dispatch();
#endif
					ThreadManager::DistributeReservedJobs();
					ThreadManager::DoGlobalQueueWork();
				}
			});
	}

#ifdef DEV_TEST
	uint64 tick = TimeUtils::GetTick64();
	while (true)
	{
		if (tick > TimeUtils::GetTick64())
			continue;
		PDH_FMT_COUNTERVALUE counterVal;
		::PdhCollectQueryData(cpuQuery);
		::PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		VIEW_INFO("CPU USAGE : {}", counterVal.doubleValue);
		tick += 1'000'000;
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