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
	�׽�Ʈ �غ��� RIO_MAX_CQ_SIZE = 134,217,728 �ε�
	���� ���� Recv, Send 2�� �� 65535 * 10���� �ϸ� CQ �ϳ��� 102�� ���� ���� �� ���� (������, 1���� �ϰ� �׽�Ʈ���� �� �� 102������)
	
	��Ŷ 1�� �ִ� ����� 8096���� �����Ѵٰ� ����, �ִ� ���� ���� 3000���̶� ���� �� ������ 16�� ������ 1���� �ִ�� ���� �� �ִ� ������ ���
	���� 1���� ��/���� ���� ����� 65535 * 10 -> 65535�� ����
	CQ ������� RQ �������� ���� �ִ�ġ�� �� �˾Ҵµ� �װ� �ƴѵ��� -> �� �κ��� �� �� ã�ƺ����� ��
	�ϴ� �̷��� ���� �� ������ �޸� ���� ���� ���ư��� �ϴµ� CPU ��뷮�� �龦�����Ѱź��� �ƹ����� �޸� Ǯ������ �Ŵ� ���� �� ���ϰ� ū ����
	���� ���������� ���� ��°ɷ� �����ߴµ� �������� ���� ������ �� �� ���������� ���� �������� ����غ��°ɷ� �ؾ��� ��...
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
