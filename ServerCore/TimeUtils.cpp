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

int64 TimeUtils::_interpolationTick = INT64_C(0);

void TimeUtils::Init(const int64& defaultTick /*= INT64_C(0)*/)
{
	if (defaultTick <= INT64_C(0))
		return;
	
	const int64& tick = GetPocoTime().timestamp().epochTime();		// ���� �ð�
	_interpolationTick = defaultTick - tick;	// DB�ð� - �����ð� = �����ð�
}

const uint64 TimeUtils::GetTick64()
{
	// DB �ð� = �����ð� + �����ð�
	return GetPocoTime().timestamp().epochTime() + _interpolationTick;
}

Poco::DateTime TimeUtils::GetPocoTime()
{
	Poco::DateTime time;
	time.makeLocal(Poco::Timezone::tzd());
	return time;
}

Poco::DateTime TimeUtils::TickToPocoTime(const int64& tick)
{
	Poco::Timestamp timestamp(static_cast<Poco::Timestamp::TimeVal>(tick) * Poco::Timestamp::resolution());
	Poco::DateTime dateTime(timestamp);
	return dateTime;
}

const int64 TimeUtils::PocoTimeToTick(const Poco::DateTime& dateTime)
{
	return dateTime.timestamp().epochTime();
}
