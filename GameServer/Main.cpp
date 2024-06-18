#include "pch.h"
#include "LogManager.h"
#include "Service.h"
#include "IOCP.h"
#include "ClientSession.h"
#include "ThreadManager.h"

#include <Poco/TimeZone.h>

int main()
{
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
					LEndTickCount = ::GetTickCount64() + 64;
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

	VIEW_INFO("{}", buffer.GetString());
	//"Driver={ODBC Driver 17 for SQL Server};Server=(LocalDB)\\MSSQLLocalDB;Database=Test;Trusted_Connection=Yes;"

	Poco::DateTime time;
	time.makeLocal(Poco::Timezone::tzd());

	return 0;
}