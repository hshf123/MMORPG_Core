#include "pch.h"
#include "TimeUtils.h"
#include <Poco/Timezone.h>

/// Creates a DateTime for the given Gregorian date and time.
///   * year is from 0 to 9999.
///   * month is from 1 to 12.
///   * day is from 1 to 31.
///   * hour is from 0 to 23.
///   * minute is from 0 to 59.
///   * second is from 0 to 60.
///   * millisecond is from 0 to 999.
///   * microsecond is from 0 to 999.

Poco::DateTime TimeUtils::_startTime = {};
uint64 TimeUtils::_updateTick = UINT64_C(0);

void TimeUtils::Init(const Poco::DateTime& serverStartTime)
{
	_startTime = serverStartTime;
}

const uint64 TimeUtils::GetTick64()
{
	return _startTime.timestamp().epochMicroseconds() + static_cast<uint64>(::clock() * 1000);
}

Poco::DateTime TimeUtils::GetPocoTime()
{
	return AddMicroseconds(_startTime, ::clock() * 1000);
}

Poco::DateTime TimeUtils::TickToPocoTime(const uint64& tick)
{
	Poco::Timestamp timestamp(static_cast<Poco::Timestamp::TimeVal>(tick));
	Poco::DateTime dateTime(timestamp);
	return dateTime;
}

const uint64 TimeUtils::PocoTimeToTick(const Poco::DateTime& dateTime)
{
	return dateTime.timestamp().epochMicroseconds();
}

Poco::DateTime TimeUtils::AddMicroseconds(const Poco::DateTime& time, const uint64& microseconds)
{
	return time + Poco::Timespan(microseconds / 1000, microseconds % 1000);
}
