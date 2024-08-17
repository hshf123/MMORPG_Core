#include "pch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "ClientSessionManager.h"

void ClientSession::OnConnected(NetAddress netAddr)
{
	ClientSessionManager::GetInstance().OnConnected(GetPacketSession());
}

void ClientSession::OnDisconnected()
{
	ClientSessionManager::GetInstance().OnDisconnected(GetPacketSession());
}

void ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	std::shared_ptr<PacketSession> session = GetPacketSession();
	PacketHeader header = *reinterpret_cast<PacketHeader*>(buffer);
	ClientPacketHandler::GetInstance().HandlePacket(session, buffer, len);
}

void ClientSession::OnSend([[maybe_unused]]int32 len)
{
	//VIEW_WRITE_INFO("Send Complete{}", len);
}
