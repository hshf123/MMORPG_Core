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
	/// <summary>
	/// DB���� �޾ƿ� ƽ -> �̰ɷ� ���� �ð� ���ϰ� ���Բ�, �ȳ����� �׳� ���� �ӽ� �ð�
	/// </summary>
	/// <param name="defaultTick"></param>
	static void Init(const Poco::DateTime& serverStartTime);

	static const uint64 GetTick64();
	static Poco::DateTime GetPocoTime();

	// EpochTime To PocoTime
	static Poco::DateTime TickToPocoTime(const uint64& tick);
	// PocoTime To EpochTime
	static const uint64 PocoTimeToTick(const Poco::DateTime& dateTime);

	static Poco::DateTime AddMicroseconds(const Poco::DateTime& time, const uint64& milliseconds);

private:
	static Poco::DateTime _startTime;
	static uint64 _updateTick;
};
