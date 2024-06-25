#include "pch.h"
#include "LogManager.h"
#include "Service.h"
#include "IOCP.h"
#include "ClientSession.h"
#include "ThreadManager.h"
#include "TestDBLoadBalancer.h"
#include "TestDBHandler.h"

/*
	1. 서버 뜰 때 초기화
	2. DB 연결 데이터 로딩
	3. 리슨, 타 서버 커넥트(있으면)
	4. 데이터 로딩 끝난 후 초기화
*/

int main()
{
	Socket::Init();
	LogManager::GetInstance().Initialize();
	LogManager::GetInstance().Launch();

	std::shared_ptr<ServerService> clientService = PoolAlloc<ServerService>(
		NetAddress(L"127.0.0.1", 9999),
		PoolAlloc<IocpCore>(),
		PoolAlloc<ClientSession>,
		10);
	ASSERT_CRASH(clientService->Start());
	for (uint64 i = INT64_C(0); i < 10; i++)
	{
		ThreadManager::GetInstance().Launch([&]()
			{
				while (true)
				{
					LEndTickCount = TimeUtils::GetTick64() + 64;
					clientService->GetIocpCore()->Dispatch(10);
					ThreadManager::DistributeReservedJobs();
					ThreadManager::DoGlobalQueueWork();
				}
			});
	}

	const std::string json = "{\"project\":\"rapidjson\",\"stars\":10}";
	rapidjson::Document d;
	d.Parse(json.c_str());

	rapidjson::Value& s = d["stars"];
	s.SetInt(s.GetInt() + 1);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);

	//Poco::DateTime time = TimeUtils::GetPocoTime();
	//int64 tick = TimeUtils::GetTick64();
	//Poco::DateTime tick2 = TimeUtils::TickToPocoTime(tick);

	TestDBHandler::GetInstance().Init();

	TestDBLoadBalancer* tdbBalancer = new TestDBLoadBalancer();
	tdbBalancer->Init("Driver={ODBC Driver 17 for SQL Server};Server=(LocalDB)\\MSSQLLocalDB;Database=Game;Trusted_Connection=Yes;", 1);
	tdbBalancer->Launch();
	
	tdbBalancer->Push(PoolAlloc<DBData>(Protocol::EDBProtocol::STDB_ServerStart, 0), TestDBHandler::GetInstance());

#ifdef DEV_TEST
	while (true);
#endif
	return 0;
}