#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"

void ClientPacketHandler::Init()
{
	PacketHandler::Init();
	RegisterHandler(Protocol::EPacketProtocol::CS_ChatRequest, [=](std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CSChatRequest>(OnCSChatRequest, session, buffer, len); });
}

bool OnCSChatRequest(std::shared_ptr<PacketSession>& session, Protocol::CSChatRequest& pkt)
{
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return false;

	return true;
}
