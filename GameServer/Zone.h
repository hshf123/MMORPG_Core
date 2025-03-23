#pragma once
#include "JobQueue.h"

class ClientSession;

class Zone : public JobQueue
{
public:
	Zone();

	void Enter(std::shared_ptr<ClientSession> cs);
	void Leave(std::shared_ptr<ClientSession> cs);

	void Broadcast(uint16 protocol, google::protobuf::Message& pkt);

private:
	static int32 ZoneID;
	int32 _zoneId = 0;
	std::unordered_set<std::shared_ptr<ClientSession>> _sessions;
};
