#pragma once
#include "Session.h"

using PacketHandlerFunc = std::function<bool(std::shared_ptr<PacketSession>&, BYTE*, int32)>;
bool Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 len);

class PacketHandler
{
public:
	virtual void Init();
	bool HandlePacket(std::shared_ptr<PacketSession>& session, BYTE* buffer, uint32 len);
	bool RegisterHandler(const uint16& protocol, PacketHandlerFunc fn);
	template<class HandlerType, class PacketType, typename = typename std::enable_if<std::is_base_of<PacketHandler, HandlerType>::value>::type>
	bool RegisterHandler(const uint16& protocol, bool (HandlerType::* handler)(std::shared_ptr<PacketSession>&, PacketType&))
	{
		return RegisterHandler(protocol, [=](std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 len)
			{
				PacketType pkt;
				if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
					return false;

				return (static_cast<HandlerType*>(this)->*handler)(session, pkt);
			});
	}
	template<class T>
	std::shared_ptr<SendBuffer> MakeSendBuffer(T& pkt) { return MakeSendBuffer(pkt, pkt.messageid()); }

protected:
	template<class PacketType, class ProcessFunc>
	bool HandlePacket(ProcessFunc func, std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<class T>
	std::shared_ptr<SendBuffer> MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		std::shared_ptr<SendBuffer>  sendBuffer = SendBufferManager::GetInstance().Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}

private:
	static PacketHandlerFunc _packetHandler[UINT16_MAX];
#ifdef _DEBUG
	static std::unordered_set<uint16> _useProtocol;
#endif
};