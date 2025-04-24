#include "pch.h"
#include "GameServer.h"
#include "Socket.h"
#include "ClientPacketHandler.h"
#include "GameDBLoadBalancer.h"
#include "GameDBHandler.h"
#include "Service.h"
#include "ClientSession.h"
#include "ThreadManager.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

void TimerJobQueue::UpdateTime()
{
	VIEW_WRITE_INFO("CPU ({:.2f}), MEOMORY ({:.2f})MB", Monitor::GetInstance().GetCPUUsage(), Monitor::GetInstance().GetMemoryUsage_MB());
	Monitor::GetInstance().PrintServerCounting();
	DoTimer(TimeUtils::OneMin, &TimerJobQueue::UpdateTime);
}

bool GameServer::Init()
{
	ThreadManager::GetInstance();	// 메인스레드 ID 생성 위해
	if (_ReadConfig() == false)
		return false;

	Socket::Init();
	LogManager::GetInstance().Initialize(_processName);
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
	std::string config = "Config.json";
	if (fs::exists(config) == false)
	{
		VIEW_ERROR("Not Found Config");
		return false;
	}
	std::ifstream file(config, std::ios::binary);
	if (file.fail())
	{
		VIEW_ERROR("Config Read Fail");
		return false;
	}
	std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	rapidjson::Document doc;
	doc.Parse(json.c_str());

	rapidjson::Value& mainValue = doc["Main"];
	_processName = mainValue["ProcessName"].GetString();
	VIEW_INFO("{}", _processName);
	_clientServiceThreadCount = mainValue["ThreadCount"].GetInt();
	VIEW_INFO("ClientService ThreadCount : {}", _clientServiceThreadCount);

	if (doc.HasMember("GameDB"))
	{
		rapidjson::Value& gdbValue = doc["GameDB"];

		if (gdbValue.HasMember("ConnectionString"))
		{
			_gameDBConnectionString = gdbValue["ConnectionString"].GetString();
		}
		else
		{
			_gameDBConnectionString =
				std::format("DRIVER=SQL SERVER;UID={};PWD={};DATABASE={};SERVER={},{};",
					gdbValue["ID"].GetString(),
					gdbValue["PWD"].GetString(),
					gdbValue["DB"].GetString(),
					gdbValue["HOST"].GetString(),
					gdbValue["PORT"].GetString());
		}
		_gameDBThreadCount = gdbValue["ThreadCount"].GetInt();
		VIEW_INFO("GameDB Connection Count : {}", _gameDBThreadCount);
	}

	if (doc.HasMember("Redis"))
	{
		rapidjson::Value& redisValue = doc["Redis"];
		_redisIP = redisValue["IP"].GetString();
		_redisPort = redisValue["Port"].GetInt();
	}

	xreserve<Job>(100'000, nullptr);
	xreserve<DBQueueData>(100'000, 0, nullptr, GameDBHandler::GetInstance());

	//{
	//	// 쓰기 테스트
	//	const std::string json = "{\"project\":\"rapidjson\",\"stars\":10}";
	//	rapidjson::Document d;
	//	d.Parse(json.c_str());
	//	rapidjson::Value& s = d["stars"];
	//	s.SetInt(s.GetInt() + 1);
	//	rapidjson::StringBuffer buffer;
	//	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	//	d.Accept(writer);
	//	std::string str = buffer.GetString();
	//}

	return true;
}

bool GameServer::_InitGameDB()
{
	if (GameDBLoadBalancer::Balancer->Init(
		_gameDBConnectionString
		, _gameDBThreadCount) == false)
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
#ifdef USE_RIO
	for (int32 i = 0; i < _clientServiceThreadCount; i++)
	{
		if (clientService->CreateRIOCQ() == false)
			return false;
	}
#endif
	_InitWorkerThread(clientService);
	return true;
}

void GameServer::_InitWorkerThread(std::shared_ptr<ServerService> service)
{
	for (int32 i = 0; i < _clientServiceThreadCount; i++)
	{
		ThreadManager::GetInstance().Launch([service]()
			{
				while (true)
				{
					LEndTickCount = TimeUtils::GetTick64() + 64;
					service->GetIocpCore()->Dispatch(10);
					ThreadManager::GetInstance().DistributeReservedJobs();
					ThreadManager::GetInstance().DoGlobalQueueWork();

					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
	}
}
