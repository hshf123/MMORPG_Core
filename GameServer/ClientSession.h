#pragma once
#include "Session.h"

class ClientSession : public PacketSession
{
public:
	void OnConnected(NetAddress netAddr) override;
	void OnDisconnected() override;
	void OnRecvPacket(BYTE* buffer, int32 len) override;
	void OnSend(int32 len) override;
};
