#include "pch.h"
#include "TestDBHandler.h"
#include "DBService.h"

void TestDBHandler::Init()
{
	DBHandler::Init();
	RegisterHandler(Protocol::EDBProtocol::STDB_ServerStart, &TestDBHandler::OnSTDBServerStart);
}

bool TestDBHandler::OnSTDBServerStart(std::shared_ptr<DBData> data, DBService* service)
{
	if (data == nullptr || service == nullptr)
		return false;

	Poco::Data::Session* dbSession = service->GetDBSession();
	if (dbSession == nullptr)
		return false;
	Poco::Data::Session& session = *dbSession;

	int serverId = 1;
	int64 dbTick = INT64_C(0);
	try
	{
		session << "{CALL spServerStart(?,?)}",
			in(serverId),
			io(dbTick),
			now;
	}
	catch (Poco::Data::ODBC::StatementException& ex)
	{
		VIEW_WRITE_ERROR("\n{}", ex.message());
	}
	catch (std::exception& e)
	{
		VIEW_WRITE_ERROR("\nDB Error : {}", e.what());
	}

	//Poco::Data::Statement select(session); // SELECT 시에 사용, range(0,1)
	/*while (!select.done())
	{
		if (select.execute() > 0)
		{
		}
	}*/

	return true;
}
