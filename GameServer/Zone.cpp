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
	_sessions.insert_or_assign(cs->GetWorkId(), cs);
}

void Zone::Leave(std::shared_ptr<ClientSession> cs)
{
	_sessions.erase(cs->GetWorkId());
}

void Zone::Broadcast(uint16 protocol, google::protobuf::Message& pkt)
{
	for (auto& cs : std::views::values(_sessions))
		cs->SendAsync(protocol, pkt);
}

std::shared_ptr<ClientSession> Zone::GetClient(int32 workId)
{
	auto findIt = _sessions.find(workId);
	if (findIt == _sessions.end())
		return nullptr;
	return findIt->second;
}

void Zone::OnSCChatResponse(std::shared_ptr<spChatReuqest> res)
{
	if (res == nullptr)
		return;
	std::shared_ptr<ClientSession> cs = GetClient(res->WorkID);
	if (cs == nullptr)
		return;

	SCChatResponse packet;
	packet.set_name(res->Name);
	packet.set_msg(res->Msg);
	cs->SendAsync(EPacketProtocol::SC_ChatResponse, packet);
}
