#include "pch.h"
#include "LogManager.h"
#include "Service.h"
#include "IOCP.h"

int main()
{
	LogManager::GetInstance().Initialize();
	LogManager::GetInstance().Launch();

	/*for (int i = 0; i < 100; i++)
		VIEW_WRITE_INFO("Hello World {}", i);

	std::shared_ptr<ServerService> loginService = PoolAlloc<ServerService>(
		NetAddress(L"127.0.0.1", 9999),
		PoolAlloc<IocpCore>(),
		nullptr,
		10);
	ASSERT_CRASH(loginService->Start());*/

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
	return 0;
}