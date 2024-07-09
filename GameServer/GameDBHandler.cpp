#include "pch.h"
#include "GameDBHandler.h"
#include "DBService.h"

void GameDBHandler::Init()
{
	DBHandler::Init();
	RegisterHandler(Protocol::EDBProtocol::SGDB_ServerStart, &GameDBHandler::OnSTDBServerStart);
}

bool GameDBHandler::OnSTDBServerStart(std::shared_ptr<DBData> data, DBService* service)
{
	if (data == nullptr || service == nullptr)
		return false;

	Poco::Data::Session* dbSession = service->GetDBSession();
	if (dbSession == nullptr)
		return false;
	Poco::Data::Session& session = *dbSession;

	int serverId = 1;
	Poco::DateTime serverStartTime;
	try
	{
		session << "{CALL spServerStart(?)}",
			in(serverId),
			into(serverStartTime),
			now;
	}
	catch (Poco::Data::ODBC::StatementException& ex)
	{
		VIEW_WRITE_ERROR(TimeUtils::GetTick64(), "\n{}", ex.message());
	}
	catch (std::exception& e)
	{
		VIEW_WRITE_ERROR(TimeUtils::GetTick64(), "\nDB Error : {}", e.what());
	}

	//Poco::Data::Statement select(session); // SELECT 시에 사용, range(0,1)
	/*while (!select.done())
	{
		if (select.execute() > 0)
		{
		}
	}*/

	TimeUtils::Init(serverStartTime);

	return true;
}
