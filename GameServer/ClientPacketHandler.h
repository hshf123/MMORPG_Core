#pragma once
#include "PacketHandler.h"
#include "Singleton.h"

class ClientPacketHandler : public PacketHandler, public RefSingleton<ClientPacketHandler>
{
public:
	void Init() override;
	bool OnCSChatRequest(std::shared_ptr<PacketSession>& session, Protocol::CSChatRequest& pkt);

};
