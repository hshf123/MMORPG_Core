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
	RIORESULT results[256] = { 0, };

	uint64 numResult = Socket::RIOEFTable.RIODequeueCompletion(GetRIOCQ(LThreadId), results, 256);
	if (numResult == 0)
		return;
	if (numResult == RIO_CORRUPT_CQ)
	{
		[[maybe_unused]]int32 errCode = ::WSAGetLastError();
		return;
	}

	for (uint64 i = 0; i < numResult; i++)
	{
		RIORecvEvent* context = reinterpret_cast<RIORecvEvent*>(results[i].RequestContext);
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
	_rioCQList = new RIO_CQ[GetMaxSessionCount()];
	for (int32 i = 0; i < GetMaxSessionCount(); i++)
	{
		_rioCQList[i] = Socket::RIOEFTable.RIOCreateCompletionQueue(163840, 0);
		if (_rioCQList[i] == RIO_INVALID_CQ)
			return false;
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
