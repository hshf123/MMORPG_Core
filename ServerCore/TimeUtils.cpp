#include "pch.h"
#include "TimeUtils.h"
#include <Poco/Timezone.h>

const uint64 TimeUtils::GetTick64()
{
	return ::GetTickCount64();
}

Poco::DateTime TimeUtils::GetLocalTime()
{
	Poco::DateTime time;
	time.makeLocal(Poco::Timezone::tzd());
	return time;
}
