#include "pch.h"
#include "DBHandler.h"

void DBHandler::Init()
{
	_useProtocol.clear();
	for (int32 i = 0; i < UINT16_MAX; i++)
		_dbHandler[i] = Handle_INVALID;
}

bool DBHandler::HandlePacket(uint16 protocolId, std::shared_ptr<DBData> data)
{
	return _dbHandler[protocolId](data);
}

bool DBHandler::RegisterHandler(const uint16& protocol, DBHandlerFunc fn)
{
	if (fn == nullptr)
		return false;
#ifdef _DEBUG
	if (_useProtocol.insert(protocol).second == false)
	{
		VIEW_WRITE_ERROR("Duplicated Protocol Detected! Check Protocol ID({})", protocol);
		return false;
	}
#endif
	_dbHandler[protocol] = fn;
	return true;
}
