#include "pch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "ClientSessionManager.h"
#include "ZoneManager.h"

void ClientSession::OnConnected(NetAddress netAddr)
{
	ClientSessionManager::GetInstance().OnConnected(GetPacketSession());
	ZoneManager::GetInstance().GetZone(GetWorkId())->DoAsync(&Zone::Enter, std::static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnDisconnected()
{
	ClientSessionManager::GetInstance().OnDisconnected(GetPacketSession());
	ZoneManager::GetInstance().GetZone(GetWorkId())->DoAsync(&Zone::Leave, std::static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	std::shared_ptr<PacketSession> session = GetPacketSession();
	PacketHeader header = *reinterpret_cast<PacketHeader*>(buffer);
	if (ClientPacketHandler::GetInstance().HandlePacket(session, buffer, len) == false)
	{
		// 로그라도 찍어야 하나?
	}
}

void ClientSession::OnSend([[maybe_unused]]int32 len)
{
	//VIEW_WRITE_INFO("Send Complete{}", len);
}

void ClientSession::Send(uint16 protocol, google::protobuf::Message& pkt)
{
	PacketSession::Send(ClientPacketHandler::GetInstance().MakeSendBuffer(pkt, protocol));
}
