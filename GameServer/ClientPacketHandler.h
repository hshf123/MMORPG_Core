#pragma once
#include "PacketHandler.h"
#include "Singleton.h"

class ClientPacketHandler : public PacketHandler, public RefSingleton<ClientPacketHandler>
{
public:
	void Init() override;
};