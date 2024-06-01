#include "pch.h"
#include "CorePch.h"
#include "LogManager.h"
#include "Service.h"
#include "IOCP.h"

int main()
{
	LogManager::GetInstance().Initialize();


	LogManager::GetInstance().Launch();

	for (int i = 0; i < 100; i++)
		VIEW_WRITE_INFO("Hello World {}", i);

	std::shared_ptr<ServerService> loginService = PoolAlloc<ServerService>(
		NetAddress(L"127.0.0.1", 9999),
		PoolAlloc<IocpCore>(),
		nullptr,
		10);
	ASSERT_CRASH(loginService->Start());

	return 0;
}