#include "pch.h"
#include "ClientSessionManager.h"

void ClientSessionManager::OnConnected(std::shared_ptr<PacketSession> session)
{
	if (session == nullptr)
		return;
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return;
	WRITE_LOCK;
	_sessions.insert(cs);
	VIEW_INFO(TimeUtils::GetTick64(), "Client Connected {}", StrUtils::ToString(cs->GetAddress().GetIpAddress()));
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

void ClientSessionManager::Broadcast(std::shared_ptr<SendBuffer> buffer)
{
	WRITE_LOCK;
	for (auto& cs : _sessions)
		cs->Send(buffer);
}
