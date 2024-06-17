#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"

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

	return true;
}