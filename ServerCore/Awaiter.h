#pragma once
#include "JobQueue.h"

struct Post
{
	Post(const JobQueue& jobQueue);

	_NODISCARD constexpr bool await_ready() const noexcept { return false; }
	constexpr void await_suspend(std::coroutine_handle<> handle) const noexcept
	{
		if (_jobQueue == nullptr)
			return;

#pragma message("TODO const_cast ªË¡¶ ø‰∏¡")
		const_cast<JobQueue*>(_jobQueue)->DoAsync([handle]()
			{
				handle.resume();
			});
	}
	constexpr void await_resume() const noexcept {}

private:
	const JobQueue* _jobQueue = nullptr;
};

class Awaiter
{
public:
	Awaiter(const JobQueue& jobQueue);

	Post PostAwait() const;

private:
	const JobQueue* _jobQueue = nullptr;
};

