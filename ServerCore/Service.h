#pragma once

#include <functional>
#include "Socket.h"

enum class ServiceType : uint8
{
	Server,
	Client
};

using SessionFactory = std::function<std::shared_ptr<class Session>(void)>;

class Session;
class IocpCore;
class Listener;

class Service : public std::enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, NetAddress address, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual bool Start() abstract;
	bool CanStart() { return _sessionFactory != nullptr; }

	virtual void CloseService();
	void SetSessionFactory(SessionFactory func) { _sessionFactory = func; }

	std::shared_ptr<Session> CreateSession();
	void AddSession(std::shared_ptr<Session> session);
	void ReleaseSession(std::shared_ptr<Session> session);
	int32 GetCurrentSessionCount() { return _sessionCount; }
	int32 GetMaxSessionCount() { return _maxSessionCount; }

public:
	ServiceType GetServiceType() { return _type; }
	NetAddress GetNetAddress() { return _netAddress; }
	std::shared_ptr<IocpCore>& GetIocpCore() { return _iocpCore; }

	RIO_CQ& GetRIOCQ(const int32& idx) { return _rioCQList[idx % GetMaxSessionCount()]; }
	void Dispatch();

protected:
	USE_LOCK;
	ServiceType _type;
	NetAddress _netAddress = {};
	std::shared_ptr<IocpCore> _iocpCore;

	std::set<std::shared_ptr<Session>> _sessions;
	int32 _sessionCount = 0;
	int32 _maxSessionCount = 0;
	SessionFactory _sessionFactory;

	RIO_CQ* _rioCQList = nullptr;	// 배열로 사용
};

class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddress, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool Start() override;
};

class ServerService : public Service
{
public:
	ServerService(NetAddress targetAddress, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ServerService() {}

	virtual bool Start() override;
	virtual void CloseService() override;

private:
	std::shared_ptr<Listener> _listener = nullptr;
};