#pragma once
#include "Singleton.h"
#include "Zone.h"

class ZoneManager : public RefSingleton<ZoneManager>
{
public:
	ZoneManager();

	std::shared_ptr<Zone> GetZone(int32 workId);

private:
	std::vector<std::shared_ptr<Zone>> _zoneList;
};
