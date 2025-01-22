#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSessionManager.h"

#include "MathUtil.h"

void ClientPacketHandler::Init()
{
	PacketHandler::Init();
	_RegisterHandler(&ClientPacketHandler::OnCSChatRequest);
	_RegisterHandler(&ClientPacketHandler::OnCSCircularSectorSkillRequest);
	_RegisterHandler(&ClientPacketHandler::OnCSBigTestRequest);
}

bool ClientPacketHandler::OnCSChatRequest(std::shared_ptr<PacketSession>& session, Protocol::CSChatRequest& pkt)
{
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return false;

	//VIEW_INFO("Recv {} : {}", pkt.name(), pkt.msg());

	Protocol::SCChatResponse packet;
	packet.set_name(pkt.name());
	packet.set_msg(pkt.msg());
	//ClientSessionManager::GetInstance().Broadcast(packet);
	cs->Send(ClientPacketHandler::GetInstance().MakeSendBuffer(packet, Protocol::SC_ChatResponse));
	return true;
}

bool ClientPacketHandler::OnCSCircularSectorSkillRequest(std::shared_ptr<PacketSession>& session, Protocol::CSCircularSectorSkillRequest& pkt)
{
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return false;

	bool ret = MathUtil::CircularSectorRangeCheck(
		pkt.theta(),
		pkt.radius(),
		Vec2(pkt.mypos().x(), pkt.mypos().y()),
		Vec2(pkt.forward().x(), pkt.forward().y()),
		Vec2(pkt.targetpos().x(), pkt.targetpos().y())
	);

	Protocol::SCCircularSectorSkillResponse packet;
	packet.set_ishit(ret);
	cs->Send(ClientPacketHandler::GetInstance().MakeSendBuffer(packet, Protocol::SC_CircularSectorSkillResponse));

	return true;
}

bool ClientPacketHandler::OnCSBigTestRequest(std::shared_ptr<PacketSession>& session, Protocol::CSBigTestRequest& pkt)
{
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return false;

	Protocol::SCBigTestResponse packet;

	for (const auto& p : pkt.list())
	{
		auto t = packet.add_list();
		t->set_a(p.a());
		t->set_b(p.b());
		t->set_c(p.c());
	}

	cs->Send(ClientPacketHandler::GetInstance().MakeSendBuffer(packet, Protocol::SC_BigTestResponse));

	return true;
}
