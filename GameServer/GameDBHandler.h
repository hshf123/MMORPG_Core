#pragma once
#include "DBHandler.h"
#include "Singleton.h"

class GameDBHandler : public DBHandler, public RefSingleton<GameDBHandler>
{
public:
	void Init() override;
	bool OnSTDBServerStart(std::shared_ptr<DBData> data, DBService* service);
};

