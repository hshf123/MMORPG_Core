#pragma once

#include "IOCP.h"
#include "Socket.h"
#include "RecvBuffer.h"

constexpr int32 WARN_SENDQUEUE_SIZE = 500;
constexpr int32 KICK_SENDQUEUE_SIZE = 1000;

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000,
	};

public:
	Session();
	virtual ~Session();

public:
	void Send(std::shared_ptr<SendBuffer> buffer);
	bool Connect();
	void Disconnect(const WCHAR* cause);

	std::shared_ptr<Service> GetService() { return _service.lock(); }
	void SetService(std::shared_ptr<Service> service) { _service = service; }

public:
	void SetNetAddr(NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }
	bool IsConnected() { return _connected; }
	int32 GetWorkId() { return _workId; }	// 패킷 흐름의 순서 보장을 위해 사용 다른 용도로 사용하지 말 것
	std::shared_ptr<Session> GetSession() { return std::static_pointer_cast<Session>(shared_from_this()); }

private:
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	void SetWorkId();

	bool RegisterConnect();
	bool RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(int32 numOfBytes);

	void HandleError(int32 errorCode);

protected:
	virtual void OnConnected([[maybe_unused]] NetAddress netAddr) {}
	virtual int32 OnRecv([[maybe_unused]] BYTE* buffer, int32 len) { return len; }
	virtual void OnSend([[maybe_unused]] int32 len) {}
	virtual void OnDisconnected() {}

private:
	std::weak_ptr<Service> _service;
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	std::atomic_bool _connected = false;
	int32 _workId;

private:
	USE_LOCK;
	RecvBuffer _recvBuffer;
	std::queue<std::shared_ptr<SendBuffer>> _sendQueue;
	std::atomic_bool _sendRegistered = false;

private:
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
};

struct PacketHeader
{
	uint16 size;
	uint16 id;
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	std::shared_ptr<PacketSession> GetPacketSession() { return std::static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32 OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) abstract;
};