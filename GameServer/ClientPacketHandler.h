#pragma once
#include "PacketHandler.h"
#include "Singleton.h"

class ClientPacketHandler : public PacketHandler, public RefSingleton<ClientPacketHandler>
{
public:
	void Init() override;
	bool OnCSChatRequest(std::shared_ptr<PacketSession>& session, Protocol::CSChatRequest& pkt);
	bool OnCSCircularSectorSkillRequest(std::shared_ptr<PacketSession>& session, Protocol::CSCircularSectorSkillRequest& pkt);
	bool OnCSBigTestRequest(std::shared_ptr<PacketSession>& session, Protocol::CSBigTestRequest& pkt);

private:
	template<class HandlerType, class PacketType, typename = typename std::enable_if<std::is_base_of<PacketHandler, HandlerType>::value>::type>
	bool _RegisterHandler(bool (HandlerType::* handler)(std::shared_ptr<PacketSession>&, PacketType&))
	{
		PacketType tempPacket;
		std::string packetName = tempPacket.GetDescriptor()->name();
		packetName.insert(2, "_");
		Protocol::EPacketProtocol protocol;
		if (Protocol::EPacketProtocol_Parse(packetName, &protocol))
		{
			return RegisterHandler(protocol, [=](std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 len)
				{
					PacketType pkt;
					if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
						return false;

					return (static_cast<HandlerType*>(this)->*handler)(session, pkt);
				});
		}

		return false;
	}
};
