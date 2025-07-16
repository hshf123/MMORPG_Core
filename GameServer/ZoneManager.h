#pragma once
#include "Singleton.h"
#include "Zone.h"
#include "Task.h"

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

	SyncTask ActorTest();

private:
	USE_LOCK;
	std::vector<std::shared_ptr<Zone>> _zoneList;
};
