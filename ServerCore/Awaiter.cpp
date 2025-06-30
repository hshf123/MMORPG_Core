#include "pch.h"
#include "Awaiter.h"
#include "JobQueue.h"

Awaiter::Awaiter(const JobQueue& jobQueue) : _jobQueue(&jobQueue)
{

}

Post Awaiter::PostAwait() const
{
	return Post(*_jobQueue);
}

Post::Post(const JobQueue& jobQueue) : _jobQueue(&jobQueue)
{
}

constexpr void Post::await_suspend(std::coroutine_handle<> handle) const noexcept
{
	if (_jobQueue == nullptr)
		return;

#pragma message("TODO const_cast ªË¡¶ ø‰∏¡")
	const_cast<JobQueue*>(_jobQueue)->DoAsync([handle]()
		{
			handle.resume();
		});
}
