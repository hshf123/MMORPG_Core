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
	for (const auto* event : std::views::values(_rioCQEventList))
		Socket::RIOEFTable.RIOCloseCompletionQueue(event->rioCQ);
#endif
}

void Service::CloseService()
{

}

std::shared_ptr<Session> Service::CreateSession()
{
	std::shared_ptr<Session> session = _sessionFactory();
	session->SetService(static_pointer_cast<Service>(shared_from_this()));

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

RIO_CQ& Service::GetRIOCQ()
{
	return _rioCQEventList[LThreadId]->rioCQ; 	// 일종의 로드밸런서..?
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

	std::shared_ptr<ServerService> service = static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{
	Service::CloseService();
}

bool ServerService::CreateRIOCQ()
{
	/*
	잘못알고 있었다 여기서 말하는 사이즈는 버퍼 사이즈가 아니라 큐 사이즈다........
	이러니 메모리가 너무 크게 잡히지....
	*/
	constexpr int PendingCount = 32;	// (Send + Recv) OutStandingCount
	constexpr int MaxClientSize = 3000;
	constexpr int CompletionQueueSize = (PendingCount * MaxClientSize);

	RIONotifyEvent* event = xnew<RIONotifyEvent>(GetIocpCore()->GetHandle());
	event->rioCQ = Socket::RIOEFTable.RIOCreateCompletionQueue(CompletionQueueSize, &event->notify);
	if (event->rioCQ == RIO_INVALID_CQ)
	{
		int32 errCode = ::GetLastError();
		VIEW_WRITE_ERROR("CompletionQueue Create Fail Err : {}", errCode);
		return false;
	}
	if (Socket::RIOEFTable.RIONotify(event->rioCQ) != ERROR_SUCCESS)
	{
		int32 errCode = ::GetLastError();
		VIEW_WRITE_ERROR("CompletionQueue Create Fail Err : {}", errCode);
		return false;
	}

	event->ownerService = shared_from_this();
	_rioCQEventList.insert_or_assign(LThreadId, event);
	return true;
}
