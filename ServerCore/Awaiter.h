#pragma once
#include "JobQueue.h"

template<class T, class... Args>
class Task
{
public:
	struct promise_type;
	using handle_type = std::coroutine_handle<promise_type>;

	Task(handle_type h, Args&&... args) : _handle(h)
	{
		_value = xnew<T>(std::forward<Args>(args)...);
	}
	handle_type handle() const { return _handle; }

	struct promise_type
	{
		std::tuple<Args...> _args; // Store args as a member variable
		promise_type(Args&&... args) : _args(std::forward<Args>(args)...) {}
		Task<T> get_return_object()
		{
			return Task<T>(handle_type::from_promise(*this), std::forward<Args>(std::get<Args>(_args))...);
		}
		std::suspend_always initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { return {}; }
		void return_value(T* value) { _value = value; }
		void unhandled_exception() {}

		T* _value;
	};

private:
	handle_type _handle;
	const T* _value;
};

template<>
class Task<void>
{
public:
	struct promise_type;
	using handle_type = std::coroutine_handle<promise_type>;

	Task(handle_type h) : _handle(h) {}
	handle_type handle() const { return _handle; }

	struct promise_type
	{
		Task get_return_object() { return Task(handle_type::from_promise(*this)); }
		std::suspend_always initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { return {}; }
		void return_void() { return; }
		void unhandled_exception() {}
	};

private:
	handle_type _handle;
};

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

