#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSessionManager.h"

void ClientPacketHandler::Init()
{
	PacketHandler::Init();
	RegisterHandler(Protocol::EPacketProtocol::CS_ChatRequest, &ClientPacketHandler::OnCSChatRequest);
}

bool ClientPacketHandler::OnCSChatRequest(std::shared_ptr<PacketSession>& session, Protocol::CSChatRequest& pkt)
{
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return false;

	VIEW_INFO(TimeUtils::GetTick64(), "Recv {} : {}", pkt.name(), pkt.msg());

	Protocol::SCChatResponse packet;
	packet.set_messageid(Protocol::EPacketProtocol::SC_ChatResponse);
	packet.set_name(pkt.name());
	packet.set_msg(pkt.msg());
	ClientSessionManager::GetInstance().Broadcast(packet);

	return true;
}