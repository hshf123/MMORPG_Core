#include "pch.h"
#include "Zone.h"
#include "ClientSession.h"

int32 Zone::ZoneID = 0;

Zone::Zone()
{
	_zoneId = ZoneID++;
}

void Zone::Enter(std::shared_ptr<ClientSession> cs)
{
	_sessions.insert(cs);
}

void Zone::Leave(std::shared_ptr<ClientSession> cs)
{
	_sessions.erase(cs);
}

void Zone::Broadcast(uint16 protocol, google::protobuf::Message& pkt)
{
	for (auto& cs : _sessions)
		cs->Send(protocol, pkt);
}
