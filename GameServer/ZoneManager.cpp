#include "pch.h"
#include "ZoneManager.h"
#include "Awaiter.h"

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

Task ZoneManager::ActorTest()
{
	// 내가 하고 싶은게
	// 1번 Zone 컨텍스트 스위칭
	uint32 startThreadId = LThreadId;
	std::shared_ptr<Zone> zone = GetZone(1);
	co_await Awaiter(*zone).PostAwait();
	// 여기서 부터
	uint32 midThreadId = LThreadId;
	// 여기까지 1번 존 구간
	// 2번 Zone 컨텍스트 스위칭
	zone = GetZone(2);
	co_await Awaiter(*zone).PostAwait();
	uint32 endThreadId = LThreadId;


	if (startThreadId == midThreadId && midThreadId == endThreadId)
	{
		//VIEW_ERROR("ZoneManager::ActorTest - Thread ID is same: {} {} {}", startThreadId, midThreadId, endThreadId);
	}
	else
	{
		VIEW_INFO("ZoneManager::ActorTest - Thread ID changed: {} -> {} -> {}", startThreadId, midThreadId, endThreadId);
	}

	co_return;
}