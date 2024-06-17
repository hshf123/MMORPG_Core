#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

void JobTimer::Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job)
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	JobData* jobData = xnew<JobData>(owner, job);

	WRITE_LOCK;

	_items.push(TimerItem{ executeTick, jobData });
}

void JobTimer::UpdateReserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job)
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	JobData* jobData = xnew<JobData>(owner, job);

	WRITE_LOCK;

	_items.push(TimerItem{ executeTick, jobData, tickAfter });
}

void JobTimer::Distribute(uint64 now)
{
	// 한 번에 1 쓰레드만 통과
	if (_distributing.exchange(true) == true)
		return;

	std::vector<TimerItem> items;
	std::vector<TimerItem> updateItems;

	{
		WRITE_LOCK;

		while (_items.empty() == false)
		{
			const TimerItem& timerItem = _items.top();
			if (now < timerItem.executeTick)
				break;

			if (timerItem.updateTick != 0)
				updateItems.push_back(timerItem);

			items.push_back(timerItem);
			_items.pop();
		}

		for (const auto& item : updateItems)
			_items.push(TimerItem{ now + item.updateTick, item.jobData, item.updateTick });
	}

	for (TimerItem& item : items)
	{
		if (std::shared_ptr<JobQueue> owner = item.jobData->owner.lock())
			owner->Push(item.jobData->job);

		if (item.updateTick == 0)
			xdelete<JobData>(item.jobData);
	}

	// 끝났으면 풀어준다
	_distributing.store(false);
}

void JobTimer::Clear()
{
	WRITE_LOCK;

	while (_items.empty() == false)
	{
		const TimerItem& timerItem = _items.top();
		xdelete<JobData>(timerItem.jobData);
		_items.pop();
	}
}