#pragma once
#include "Session.h"
#include "JobQueue.h"
#include "Singleton.h"

class ClientSession : public PacketSession
{
public:
	ClientSession() {}
	virtual ~ClientSession() {}

	void OnConnected(NetAddress netAddr) override;
	void OnDisconnected() override;
	void OnRecvPacket(BYTE* buffer, int32 len) override;
	void OnSend(int32 len) override;

public:
	void SendAsync(uint16 protocol, google::protobuf::Message& pkt);
};

class ClientSessionSender : public JobQueue, public PtrSingleton<ClientSessionSender>
{
public:
	void SendAsync(std::shared_ptr<ClientSession> cs, std::shared_ptr<SendBuffer> buffer);
};