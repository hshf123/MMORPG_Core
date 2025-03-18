#include "pch.h"
#include "GameDBHandler.h"
#include "DBService.h"
#include "GameDBData.h"
#include "GlobalQueue.h"
#include "JobQueue.h"
#include "GameDBLoadBalancer.h"

void GameDBHandler::Init()
{
	DBHandler::Init();
	RegisterHandler(EDBProtocol::SGDB_ServerStart, &GameDBHandler::OnSGDBServerStart);
	RegisterHandler(EDBProtocol::SGDB_ChatRequest, &GameDBHandler::OnSGDBChatRequest);
}

bool GameDBHandler::OnSGDBServerStart(std::shared_ptr<DBData> data)
{
	GetSession();

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
		VIEW_WRITE_ERROR("\n{}", StrUtils::ToString(ex.message().c_str()));
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

	TimeUtils::Init(serverStartTime);

	return true;
}

bool GameDBHandler::OnSGDBChatRequest(std::shared_ptr<DBData> data)
{
	GetSession();
	std::shared_ptr<spChatReuqest> req = static_pointer_cast<spChatReuqest>(data);
	if (req == nullptr)
		return false;

	try
	{
		session << "{CALL spChatReuqest(?,?)}",
			out(req->Result),
			in(req->SessionID),
			now;
	}
	catch (Poco::Data::ODBC::StatementException& ex)
	{
		VIEW_WRITE_ERROR("\n{}", StrUtils::ToString(ex.message().c_str()));
	}
	catch (std::exception& e)
	{
		VIEW_WRITE_ERROR("\nDB Error : {}", e.what());
	}

	// 로직 스레드로 전환
	GlobalQueue::GetInstance().PushDBData(data);
	return true;
}
