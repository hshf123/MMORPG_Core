#include "pch.h"
#include "PC.h"
#include "Zone.h"

PC::PC(std::shared_ptr<Zone> zone) : _zone(zone)
{
}

int32 PC::GetZoneID() const
{
	std::shared_ptr<Zone> zone = _zone.lock();
	if (zone == nullptr)
		return -1;
	return zone->GetZoneID();
}