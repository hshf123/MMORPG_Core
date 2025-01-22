#pragma once
#include "ClientSession.h"
#include "Singleton.h"
#include "ClientPacketHandler.h"

class ClientSessionManager : public RefSingleton<ClientSessionManager>
{
public:
	void OnConnected(std::shared_ptr<PacketSession> session);
	void OnDisconnected(std::shared_ptr<PacketSession> session);

	template<class T>
	void Broadcast(uint16 protocol, T& pkt)
	{
		WRITE_LOCK;
		for (auto& cs : _sessions)
			cs->Send(protocol, pkt);
	}

private:
	USE_LOCK;	// SessionLock;
	std::unordered_set<std::shared_ptr<ClientSession>> _sessions;
};
