#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSessionManager.h"

#define PROTO(id) Protocol::EPacketProtocol::id
#define HANDLER(handler) &ClientPacketHandler::handler

void ClientPacketHandler::Init()
{
	PacketHandler::Init();
	RegisterHandler(PROTO(CS_ChatRequest), HANDLER(OnCSChatRequest));
}

bool ClientPacketHandler::OnCSChatRequest(std::shared_ptr<PacketSession>& session, Protocol::CSChatRequest& pkt)
{
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return false;

	//VIEW_INFO("Recv {} : {}", pkt.name(), pkt.msg());

	Protocol::SCChatResponse packet;
	packet.set_messageid(Protocol::EPacketProtocol::SC_ChatResponse);
	packet.set_name(pkt.name());
	packet.set_msg(pkt.msg());
	//ClientSessionManager::GetInstance().Broadcast(packet);
	cs->Send(ClientPacketHandler::GetInstance().MakeSendBuffer(packet));
	return true;
}