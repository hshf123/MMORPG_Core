#include "pch.h"
#include "ZoneManager.h"

ZoneManager::ZoneManager()
{
	_zoneList.resize(50);
	for (int32 i = 0; i < 50; i++)
		_zoneList[i] = PoolAlloc<Zone>();
}

std::shared_ptr<Zone> ZoneManager::GetZone(int32 workId)
{
	return _zoneList[workId % 50];
}
