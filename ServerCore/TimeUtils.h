#pragma once

#ifndef POCO_STATIC
#	define POCO_STATIC 1
#endif
#pragma warning (push)

#include <Poco/Data/Time.h>
#pragma warning (pop)

class TimeUtils
{
public:
	// DB���� �޾ƿ� ƽ -> �̰ɷ� ���� �ð� ���ϰ� ������
	static void Init(const int64& defaultTick = INT64_C(0));

	static const uint64 GetTick64();
	static Poco::DateTime GetPocoTime();

	// EpochTime To PocoTime
	static Poco::DateTime TickToPocoTime(const int64& tick);
	// PocoTime To EpochTime
	static const int64 PocoTimeToTick(const Poco::DateTime& dateTime);

private:
	static int64 _interpolationTick;
};
