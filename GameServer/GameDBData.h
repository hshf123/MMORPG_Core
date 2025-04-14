#pragma once
#include "DBHandler.h"

class spChatRequest : public DBData
{
public:
	int32 Result = -3;
	int32 SessionID = 0;

	std::string Name = {};
	std::string Msg = {};
};

class spSkillUse : public DBData
{
public:
	int32 Result = -3;
	int32 SessionID = 0;

	bool IsHit = false;
};