#pragma once
#include "DBHandler.h"
#include "Singleton.h"

class TestDBHandler : public DBHandler, public RefSingleton<TestDBHandler>
{
public:
	void Init() override;
	bool OnSTDBServerStart(std::shared_ptr<DBData> data, DBService* service);
};

