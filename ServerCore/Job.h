#pragma once
#include <functional>

using CallbackType = std::function<void()>;

class Task
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

/// <summary>
/// 게임 로직 일감, 수행할 함수 포인터와 인자를 가짐
/// </summary>
class Job
{
public:
	Job(CallbackType&& callback) : _callback(std::move(callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		_callback = [owner, memFunc, args...]()
			{
				(owner.get()->*memFunc)(args...);
			};
	}

	template<typename T, typename Ret, typename... Args>
	Job(T owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		_callback = [owner, memFunc, args...]()
			{
				(owner.*memFunc)(args...);
			};
	}

	void Execute()
	{
		_callback();
	}

private:
	CallbackType _callback;
};

