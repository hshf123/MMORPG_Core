#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"
#include "ThreadManager.h"

Service::Service(ServiceType type, NetAddress address, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount /*= 1*/)
	: _type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{
#ifdef USE_RIO
	for (const auto& cq : _rioCQList)
		Socket::RIOEFTable.RIOCloseCompletionQueue(cq);
#endif
}

void Service::CloseService()
{

}

std::shared_ptr<Session> Service::CreateSession()
{
	std::shared_ptr<Session> session = _sessionFactory();
	session->SetService(shared_from_this());

	if (_iocpCore->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(std::shared_ptr<Session> session)
{
	WRITE_LOCK;
	_sessions.insert(session);
	_sessionCount++;
}

void Service::ReleaseSession(std::shared_ptr<Session> session)
{
	WRITE_LOCK;
	ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;
}

void Service::Dispatch()
{
	RIORESULT results[RIO_DISPATCH_RESULT_COUNT] = { 0, };

	uint64 numResult = INT64_C(0);
	{
		WRITE_LOCK;
		numResult = Socket::RIOEFTable.RIODequeueCompletion(GetRIOCQ(), results, RIO_DISPATCH_RESULT_COUNT);
		if (numResult == 0)
			return;
		if (numResult == RIO_CORRUPT_CQ)
		{
			int32 errCode = ::GetLastError();
			VIEW_WRITE_ERROR("RIO Dispatch Error : {}", errCode);
			return;
		}
	}

	for (uint64 i = 0; i < numResult; i++)
	{
		RIOEvent* context = reinterpret_cast<RIOEvent*>(results[i].RequestContext);
		std::shared_ptr<IocpObject> iocpObject = context->owner;
		iocpObject->Dispatch(context, results[i].BytesTransferred);
	}
}

ClientService::ClientService(NetAddress targetAddress, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount /*= 1*/)
	: Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount)
{

}

bool ClientService::Start()
{
	if (CanStart() == false)
		return false;

	const int32 sessionCount = GetMaxSessionCount();
	for (int32 i = 0; i < sessionCount; i++)
	{
		std::shared_ptr<Session> session = CreateSession();
		if (session->Connect() == false)
			return false;
	}

	return true;
}

ServerService::ServerService(NetAddress targetAddress, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount /*= 1*/)
	: Service(ServiceType::Server, targetAddress, core, factory, maxSessionCount)
{

}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	_listener = PoolAlloc<Listener>();
	if (_listener == nullptr)
		return false;

#ifdef USE_RIO
	/*
	테스트 해보니 RIO_MAX_CQ_SIZE = 134,217,728 인데
	지금 버퍼 Recv, Send 2개 다 65535 * 10으로 하면 CQ 하나당 102개 까지 받을 수 있음 (실제로, 1개로 하고 테스트했을 때 딱 102까지임)
	
	패킷 1개 최대 사이즈를 8096으로 제한한다고 가정, 최대 유저 수는 3000명이라 했을 때 스레드 16개 돌려서 1번에 최대로 들어올 수 있는 사이즈 계산
	세션 1개의 송/수신 버퍼 사이즈를 65535 * 10 -> 65535로 변경
	CQ 사이즈는 RQ 사이즈의 합의 최대치인 줄 알았는데 그게 아닌듯함 -> 이 부분은 좀 더 찾아봐야할 듯
	일단 이렇게 했을 때 돌리면 메모리 누수 없이 돌아가긴 하는데 CPU 사용량이 들쑥날쑥한거보면 아무래도 메모리 풀링에서 거는 락이 좀 부하가 큰 듯함
	기존 락프리에서 락을 잡는걸로 변경했는데 락프리로 가면 성능이 좀 더 나아지는지 보고 락프리를 사용해보는걸로 해야할 듯...
	*/
	constexpr int PacketSize = 8096;
	constexpr int MaxClientSize = 3000;
	constexpr int ThreadCount = 16;
	constexpr int CompletionQueueSize = (PacketSize * MaxClientSize) / ThreadCount;

	for (int32 i = 0; i < ThreadCount; i++)
	{
		_rioCQList.push_back(Socket::RIOEFTable.RIOCreateCompletionQueue(CompletionQueueSize, nullptr));
		if (_rioCQList[i] == RIO_INVALID_CQ)
		{
			int32 errCode = ::GetLastError();
			VIEW_WRITE_ERROR("CompletionQueue Create Fail Err : {}", errCode);
			return false;
		}
	}
#endif

	std::shared_ptr<ServerService> service = static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{
	Service::CloseService();
}
