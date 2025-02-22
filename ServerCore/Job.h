#pragma once
#include <functional>

using CallbackType = std::function<void()>;

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

