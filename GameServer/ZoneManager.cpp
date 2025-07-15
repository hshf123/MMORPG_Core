#include "pch.h"
#include "ZoneManager.h"
#include "Awaiter.h"
#include "ChainFunc.h"

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

SyncTask ZoneManager::ActorTest()
{
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
	// 스레드 아이디가 바뀌었는지 확인
	// 함수 하나 안에서 스레드 아이디가 바뀐다
	if (startThreadId != midThreadId || midThreadId != endThreadId)
		VIEW_INFO("ZoneManager::ActorTest - Thread ID changed: {} -> {} -> {}", startThreadId, midThreadId, endThreadId);

	// TODO 1번 2번 3번을 모두 비동기로 처리하려면 여기에 JobQueue를 섞으면 됨
	// 그러면 언젠가 1번이 실행되고 이어서 바로 2번 실행이 아니라
	// 그 뒤에 언젠가 또 2번이 실행되고 3번도 언젠가 이어서 실행된다~
	// 언제 끝날지는 알 수 없지만 반드시 이어서 실행된다는 순서보장은 할 수 있게됨
	// 멀티스레드 순서보장 용도 (빠른 리턴을 바라면 안됨)
	
	ChainFunc<int32> chaining = ChainFuncMaker::GetInstance().Make<int32>([]()
		{
			// 1번 실행 후~
			return 1;
		}).Chain([](std::shared_ptr<int32> ret)
			{
				// 1번 값으로 2번 실행 후~
				*ret += 2;
			}).Chain([](std::shared_ptr<int32> ret)
				{
					// 1 + 2번 결과로 3번 실행까지
					*ret += 3;
					VIEW_INFO("Chain : {}", *ret);
					// 원래도 체인에 걸려있는 값은 안건드리는게 좋긴한데
					// 비동기로 가게되면 std::future::get 마냥 건드려도 되는지 확인하고 꺼내오는게 필요함 그냥 체이닝 함수 안에서만 건드리게끔 설계하는게 좋을듯
				});

	co_return;
}
