#pragma once
#include "JobQueue.h"

struct Post
{
	Post(std::shared_ptr<JobQueue> jobQueue);
	_NODISCARD constexpr bool await_ready() const noexcept { return false; }
	void await_suspend(std::coroutine_handle<> handle) const noexcept
	{
		std::shared_ptr<JobQueue> jobQueue = _jobQueue.lock();
		if (jobQueue == nullptr)
			return;
		jobQueue->DoAsync([handle]() { handle.resume(); });
	}
	constexpr void await_resume() const noexcept {}

private:
	std::weak_ptr<JobQueue> _jobQueue;
};

class Awaiter
{
public:
	Awaiter(std::shared_ptr<JobQueue> jobQueue);

	Post PostAwait() const;

private:
	std::weak_ptr<JobQueue> _jobQueue;
};
