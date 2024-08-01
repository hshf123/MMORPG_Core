#include "pch.h"
#include "DBHandler.h"
#include "DBService.h"

void DBHandler::Init()
{
#ifdef DEV_TEST
	_useProtocol.clear();
#endif
	for (int32 i = 0; i < UINT16_MAX; i++)
		_dbHandler[i] = Handle_INVALID;
}

bool DBHandler::HandleData(uint16 protocolId, std::shared_ptr<DBData> data, DBService* service)
{
	return _dbHandler[protocolId](data, service);
}

bool DBHandler::RegisterHandler(const uint16& protocol, DBHandlerFunc fn)
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
	_dbHandler[protocol] = fn;
	return true;
}

bool Handle_INVALID(std::shared_ptr<DBData> data, DBService* service)
{
	if (data == nullptr)
		return false;
	VIEW_WRITE_ERROR("Invalid Inner Data dected ProtocolID({}), WorkID({})", data->ProtocolID, data->WorkID);
	return false;
}
