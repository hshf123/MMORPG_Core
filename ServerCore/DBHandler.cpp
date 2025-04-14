#include "pch.h"
#include "DBHandler.h"
#include "DBService.h"
#include "JobQueue.h"

void DBData::ProcessDBWorking()
{
	if (Owner == nullptr || ResponseJob == nullptr)
	{
		Owner = nullptr;
		ResponseJob = nullptr;
		return;
	}

	Owner->Push(ResponseJob, true);
	ResponseJob = nullptr;

	Monitor::GetInstance().DecProcessCount();
}

void DBHandler::Init()
{
#ifdef DEV_TEST
	_useProtocol.clear();
#endif
	for (int32 i = 0; i < UINT16_MAX; i++)
		_dbHandler[i] = Handle_INVALID;
}

bool DBHandler::HandleData(uint16 protocolId, std::shared_ptr<DBData> data)
{
	return _dbHandler[protocolId](data);
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

bool Handle_INVALID(std::shared_ptr<DBData> data)
{
	if (data == nullptr)
		return false;
	VIEW_WRITE_ERROR("Invalid Inner Data dected ProtocolID({}), WorkID({})", data->ProtocolID, data->WorkID);
	return false;
}
