#include "pch.h"
#include "ZoneManager.h"
#include "Awaiter.h"
#include "ChainFunc.h"
#include "PC.h"

ZoneManager::ZoneManager()
{
	WRITE_LOCK;
	_zoneList.resize(50);
	for (int32 i = 0; i < 50; i++)
		_zoneList[i] = PoolAlloc<Zone>();
}

std::shared_ptr<Zone> ZoneManager::GetZone(int32 workId)
{
	READ_LOCK;
	return _zoneList[workId % 50];
}

SyncTask ZoneManager::ActorTest()
{
	// 이 함수 반환이 SyncTask인 이유
	// 
	// co_await 에서 뱉는 Post는 await_ready가 항상 false라 어디선가 다시 실행시켜주기 전까지는 멈춤 상태가 됨
	// 이 함수를 호출하는 부분이 비동기가 되길 원함 -> initial_suspend가 std::suspend_always를 반환하면 됨
	// 근데 이 때 받은 코루틴 핸들을 어디선가 resume()시켜서 다시 이어서 실행하도록 해야함 근데 코어 말고 코루틴 핸들을 컨텐츠코드에서 접근하는걸 원하지 않음
	// 
	// 그래서 채택한 방법
	// 어차피 잡큐에 넣으면 그 자체가 비동기임 (경우에 따라 바로 실행되긴 하지만 상관없음 무조건 나중에 실행되는게 목적이 아님)
	// 이미 잡큐에 넣는 순간 비동기라 한 번 더 비동기 처리를 해줄 이유가 없음
	// SyncTask는 initial_suspend가 std::suspend_never을 반환하기 때문에 실행될 때 기다리지 않고 바로 ActorTest함수가 실행됨
	// 만약 std::suspend_never가 아니었다면 잡큐처리(비동기) + 코루틴처리(비동기)로 인해 쓸데없이 늦어지게 됨
	//
	// 물론 지금은 테스트 용도라 이거 잡큐에서 실행안되고 그냥 바로 메인루프에서 실행되긴 함 (말 그대로 테스트 용도니까)
	// 코루틴을 사용할거면 이런식으로 사용해라~ 임

	// 1번 Zone 컨텍스트 스위칭
	uint32 startThreadId = LThreadId;
	std::shared_ptr<Zone> zone = GetZone(1);
	co_await Awaiter(zone).PostAwait();

	// 여기서 부터
	uint32 midThreadId = LThreadId;
	// 여기까지 1번 존 구간

	// 2번 Zone 컨텍스트 스위칭
	zone = GetZone(2);
	co_await Awaiter(zone).PostAwait();

	uint32 endThreadId = LThreadId;
	// 스레드 아이디가 바뀌었는지 확인
	// 함수 하나 안에서 스레드 아이디가 바뀐다
	if (startThreadId != midThreadId || midThreadId != endThreadId)
	{
		// VIEW_INFO("ZoneManager::ActorTest - Thread ID changed: {} -> {} -> {}", startThreadId, midThreadId, endThreadId);
	}

	// 1번 2번 3번을 모두 비동기로 처리하려면 여기에 JobQueue를 섞으면 됨 -> 섞음
	// 그러면 언젠가 1번이 실행되고 이어서 바로 2번 실행이 아니라
	// 그 뒤에 언젠가 또 2번이 실행되고 3번도 언젠가 이어서 실행된다~
	// 언제 끝날지는 알 수 없지만 반드시 이어서 실행된다는 순서보장은 할 수 있게됨
	// 멀티스레드 순서보장 용도 (빠른 리턴을 바라면 안됨)
	// 결과 6 잘 나옴 소멸자 호출도 잘됨 (어디서 물고 안놓거나 서버가 터지거나 하진 않네
	auto chain = ChainFuncMaker::GetInstance().Make<int32>([]()
		{
			// 1번 실행 후~
			return 1;
		})->Chain([](int32 ret)
			{
				// 1번 값으로 2번 실행 후~
				return int32(ret + 2);
			})->ChainAsync([](int32 ret)
				{
					// 1 + 2번 결과로 3번 실행까지
					ret += 3;
					VIEW_INFO("Chain : {}", ret);
					// 원래도 체인에 걸려있는 값은 안건드리는게 좋긴한데
					// 비동기로 가게되면 std::future::get 마냥 건드려도 되는지 확인하고 꺼내오는게 필요함 그냥 체이닝 함수 안에서만 건드리게끔 설계하는게 좋을듯
				});

	ChainFuncMaker::GetInstance().Make<std::shared_ptr<PC>>([zone]() 
		{
			// ex) PC 인증절차 다 거치고 뭐 DB 호출할거 다 하고 여기서 이제 PC를 만든다.
			return PoolAlloc<PC>(zone);
		})->Chain([](std::shared_ptr<PC> pc)
			{
				// 만들어진 PC를 PCManager에 둬도 되고 PCManager에 안둘거면 Zone에라도 등록 시킨다 치자 (생명유지를 위해)
				// zone->DoAsync(&Zone::EnterPC, pc);

				// 여기서 함정은 ChainFunc, Zone, PC 다 Actor임...
				// 그래서 서로 건드릴 때 마다 스위칭이 필요한데...

				// 방법 1. ChainFunc를 JobQueue 상속 받지 말고 생성자를 JobQueue 받는 버전 안받는 버전 만들어서 1번이라도 스위칭 줄일 수 있도록?
				// 방법 2. 그냥 코루틴만 써.... 솔직히 코루틴 없으면 이거 개량시켜서 쓸만한데 코루틴 있으면 이거 없어도 상관 없어보이는데...
				VIEW_INFO("My Zone ID : {}", pc->GetZoneID());
			});

	co_return;
}
