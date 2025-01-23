#include "pch.h"
#include "ClientSessionManager.h"

void ClientSessionManager::OnConnected(std::shared_ptr<PacketSession> session)
{
	if (session == nullptr)
		return;
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return;
	VIEW_INFO("Client Connected {}", StrUtils::ToString(cs->GetAddress().GetIpAddress()));
	WRITE_LOCK;
	_sessions.insert(cs);
}

void ClientSessionManager::OnDisconnected(std::shared_ptr<PacketSession> session)
{
	if (session == nullptr)
		return;
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return;
	WRITE_LOCK;
	_sessions.erase(cs);
}
