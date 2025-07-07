#pragma once

class SyncTask
{
public:
	struct promise_type;
	using handle_type = std::coroutine_handle<promise_type>;
	struct promise_type
	{
		SyncTask get_return_object() { return SyncTask(); }
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { return {}; }
		void return_void() {}
		void unhandled_exception() {}
	};
};

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