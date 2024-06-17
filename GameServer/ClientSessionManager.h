#pragma once
#include "ClientSession.h"
#include "Singleton.h"

class ClientSessionManager : public RefSingleton<ClientSessionManager>
{
public:
	void OnConnected(std::shared_ptr<PacketSession> session);
	void OnDisconnected(std::shared_ptr<PacketSession> session);

	void Broadcast(std::shared_ptr<SendBuffer> buffer);

private:
	USE_LOCK;	// SessionLock;
	std::unordered_set<std::shared_ptr<ClientSession>> _sessions;
};

