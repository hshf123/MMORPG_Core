#include "pch.h"
#include "Zone.h"
#include "ClientSession.h"
#include "ZoneManager.h"

int32 Zone::ZoneID = 0;

Zone::Zone()
{
	_zoneId = ZoneID++;
}

void Zone::Enter(std::shared_ptr<ClientSession> cs)
{
	_sessions.insert_or_assign(cs->GetWorkId(), cs);
}

void Zone::Leave(std::shared_ptr<ClientSession> cs)
{
	_sessions.erase(cs->GetWorkId());
}

void Zone::Broadcast(uint16 protocol, google::protobuf::Message& pkt)
{
	for (auto& cs : std::views::values(_sessions))
		cs->Send(protocol, pkt);
}

std::shared_ptr<ClientSession> Zone::GetClient(int32 workId)
{
	auto findIt = _sessions.find(workId);
	if (findIt == _sessions.end())
		return nullptr;
	return findIt->second;
}

void Zone::OnSCChatResponse(std::shared_ptr<spChatRequest> res)
{
	if (res == nullptr)
		return;
	std::shared_ptr<ClientSession> cs = GetClient(res->WorkID);
	if (cs == nullptr)
		return;

	SCChatResponse packet;
	packet.set_name(res->Name);
	packet.set_msg(res->Msg);
	cs->Send(EPacketProtocol::SC_ChatResponse, packet);
}

void Zone::OnSCCircularSectorSkillResponse(std::shared_ptr<spSkillUse> res)
{
	SCCircularSectorSkillResponse packet;
	packet.set_ishit(res->IsHit);
	Broadcast(EPacketProtocol::SC_CircularSectorSkillResponse, packet);
}
