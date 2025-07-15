#include "pch.h"
#include "Awaiter.h"

Awaiter::Awaiter(std::shared_ptr<JobQueue> jobQueue) : _jobQueue(jobQueue)
{

}

Post Awaiter::PostAwait() const
{
	return Post(_jobQueue.lock());
}

Post::Post(std::shared_ptr<JobQueue> jobQueue) : _jobQueue(jobQueue)
{
}
