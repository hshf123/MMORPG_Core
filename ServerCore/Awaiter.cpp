#include "pch.h"
#include "Awaiter.h"

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
