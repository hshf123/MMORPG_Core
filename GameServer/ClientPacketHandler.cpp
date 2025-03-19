#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSessionManager.h"
#include "GameDBData.h"
#include "MathUtil.h"
#include "GameDBLoadBalancer.h"

void ClientPacketHandler::Init()
{
	PacketHandler::Init();
	_RegisterHandler(&ClientPacketHandler::OnCSChatRequest);
	_RegisterHandler(&ClientPacketHandler::OnCSCircularSectorSkillRequest);
	_RegisterHandler(&ClientPacketHandler::OnCSBigTestRequest);
}

bool ClientPacketHandler::OnCSChatRequest(std::shared_ptr<PacketSession>& session, CSChatRequest& pkt)
{
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return false;

	std::shared_ptr<spChatReuqest> req = PoolAlloc<spChatReuqest>();
	req->SessionID = cs->GetWorkId();
	req->Name = pkt.name();
	req->Msg = pkt.msg();
	//req->Response = [=](std::shared_ptr<DBData> data)
	//	{
	//		SCChatResponse packet;
	//		packet.set_name(req->Name);
	//		packet.set_msg(req->Msg);
	//		ClientSessionManager::GetInstance().Broadcast(EPacketProtocol::SC_ChatResponse, packet);
	//		req->Response = nullptr;
	//		return true;
	//	};
	GameDBLoadBalancer::Balancer->Push(cs->GetWorkId(), EDBProtocol::SGDB_ChatRequest, req);

	SCChatResponse packet;
	packet.set_name(req->Name);
	packet.set_msg(req->Msg);
	ClientSessionManager::GetInstance().Broadcast(EPacketProtocol::SC_ChatResponse, packet);
	return true;
}

bool ClientPacketHandler::OnCSCircularSectorSkillRequest(std::shared_ptr<PacketSession>& session, CSCircularSectorSkillRequest& pkt)
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

	SCCircularSectorSkillResponse packet;
	packet.set_ishit(ret);
	cs->Send(EPacketProtocol::SC_CircularSectorSkillResponse, packet);
	return true;
}

bool ClientPacketHandler::OnCSBigTestRequest(std::shared_ptr<PacketSession>& session, CSBigTestRequest& pkt)
{
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return false;

	SCBigTestResponse packet;

	for (const auto& p : pkt.list())
	{
		auto t = packet.add_list();
		t->set_a(p.a());
		t->set_b(p.b());
		t->set_c(p.c());
	}

	cs->Send(EPacketProtocol::SC_BigTestResponse, packet);
	return true;
}
