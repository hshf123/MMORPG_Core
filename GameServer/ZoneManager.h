#pragma once
#include "Singleton.h"
#include "Zone.h"
#include "Awaiter.h"

class TestActor
{
public:
	TestActor(int32 a, int32 b) : _a(a), _b(b) {}

private:
	int32 _a;
	int32 _b;
};

class ZoneManager : public RefSingleton<ZoneManager>
{
public:
	ZoneManager();

	std::shared_ptr<Zone> GetZone(int32 workId);

	Task<void> ActorTest();
	Task<std::shared_ptr<TestActor>> ActorTest2();

private:
	std::vector<std::shared_ptr<Zone>> _zoneList;
};
