#include "pch.h"
#include "PacketHandler.h"

PacketHandlerFunc PacketHandler::_packetHandler[UINT16_MAX];
#ifdef DEV_TEST
std::unordered_set<uint16> PacketHandler::_useProtocol;
#endif
void PacketHandler::Init()
{
#ifdef DEV_TEST
	_useProtocol.clear();
#endif
	for (int32 i = 0; i < UINT16_MAX; i++)
		_packetHandler[i] = Handle_INVALID;
}

bool PacketHandler::HandlePacket(std::shared_ptr<PacketSession>& session, BYTE* buffer, uint32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	return _packetHandler[header->id](session, buffer, len);
}

bool PacketHandler::RegisterHandler(const uint16& protocol, PacketHandlerFunc fn)
{
	if (fn == nullptr)
		return false;
#ifdef DEV_TEST
	if (_useProtocol.insert(protocol).second == false)
	{
		VIEW_WRITE_ERROR("Duplicated Protocol Detected! Check Protocol ID({})", protocol);
		return false;
	}
#endif
	_packetHandler[protocol] = fn;
	return true;
}

bool Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	if (header == nullptr)
		return false;

	VIEW_WRITE_ERROR("Invalid Pakcet dected ID({}), IP({})", header->id, StrUtils::ToString(session->GetAddress().GetIpAddress()));
	return false;
}
