#pragma once
#include "DBHandler.h"

class spChatRequest : public DBData
{
public:
	~spChatRequest()
	{

	}

	int32 Result = -3;
	int32 SessionID = 0;

	std::string Name = {};
	std::string Msg = {};
	int64 Ping = INT64_C(0);
};