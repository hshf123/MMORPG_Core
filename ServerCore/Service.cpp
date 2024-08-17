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
		numResult = Socket::RIOEFTable.RIODequeueCompletion(GetRIOCQ(LThreadId), results, RIO_DISPATCH_RESULT_COUNT);
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
	
	근데 너무 큼... 현재 사양으로는 9개 까지 만들 수 있는데 메모리를 너무 잡아먹음
	RIOCreateCompletionQueue 이거 하나에 3.3GB 말이됨??????
	*/
	constexpr int cqCount = 1;
	_rioCQList.resize(cqCount);
	for (int32 i = 0; i < cqCount; i++)
	{
		_rioCQList[i] = Socket::RIOEFTable.RIOCreateCompletionQueue(163840, 0);
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
