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
		/*
		USE [Game]
		GO
		SET ANSI_NULLS ON
			GO
			SET QUOTED_IDENTIFIER ON
			GO

			ALTER PROCEDURE[dbo].[spServerStart]
			@ServerID INT
			AS
			BEGIN
			SET NOCOUNT ON;

		BEGIN TRY
			DECLARE @TimeStamp DATETIMEOFFSET = SYSDATETIMEOFFSET();

		DELETE FROM[dbo].[ServerState] WHERE[ServerID] = @ServerID;

		INSERT INTO[dbo].[ServerState]([ServerID], [State], [Time])
			VALUES(@ServerID, 1, @TimeStamp);

		SELECT @TimeStamp;
		END TRY
			BEGIN CATCH
			ROLLBACK;
		INSERT[dbo].[TProcedureError] DEFAULT VALUES;
		THROW;
		END CATCH
			END;

		*/
		Poco::Data::Statement state(session);
		state << std::format("EXEC spServerStart {}", serverId), into(serverStartTime);
		state.execute();
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
	/*
	while (!select.done())
	{
		if (select.execute() > 0)
		{
		}
	}
	*/

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
		/*
		USE [Game]
		GO
		SET ANSI_NULLS ON
			GO
			SET QUOTED_IDENTIFIER ON
			GO

			ALTER PROCEDURE[dbo].[spChatRequest]
			@Result INT OUTPUT,
			@WorkID INT
			AS
			BEGIN
			SET NOCOUNT ON;

		SET @Result = 1;
		--현재 시간을 가져오기
			DECLARE @CurrentTime DATETIME = GETDATE();

		--WorkID 존재 여부 확인
			IF EXISTS(SELECT 1 FROM[dbo].[ChatTime] WHERE WorkID = @WorkID)
			BEGIN
			-- WorkID가 존재하면 Count 증가 및 Time 업데이트
			UPDATE[dbo].[ChatTime]
			SET Count = Count + 1,
			Time = @CurrentTime
			WHERE WorkID = @WorkID;
		END
			ELSE
			BEGIN
			-- WorkID가 없으면 새 행 삽입
			INSERT INTO ChatTime(WorkID, Time, Count)
			VALUES(@WorkID, @CurrentTime, 1);
		END

			IF @@ROWCOUNT = 0
			BEGIN
			SET @Result = -1;
		END
			ELSE
			BEGIN
			SET @Result = 0;
		END
			END;
		*/

		Poco::Data::Statement state(session);
		state << std::format("							\
			DECLARE @result INT;						\
			EXEC spChatRequest @result OUTPUT, {};		\
			SELECT @result;								\
		"
			, req->SessionID)
			, into(req->Result);
		state.execute();

		//session << "EXEC spChatRequest ?,?",
		//	out(req->Result),
		//	in(req->SessionID),
		//	now;
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
	data->ProcessDBWorking();
	return true;
}
