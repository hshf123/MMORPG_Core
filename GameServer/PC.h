#pragma once
#include "JobQueue.h"

class Zone;
class PC : public JobQueue
{
public:
	PC(std::shared_ptr<Zone> zone);

	int32 GetZoneID() const;

private:
	std::weak_ptr<Zone> _zone;
};
