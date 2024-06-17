#pragma once
#include <Poco/Data/Time.h>

class TimeUtils
{
public:
	static const uint64 GetTick64();
	static Poco::DateTime GetLocalTime();
};
