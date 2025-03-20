#pragma once

/// <summary>
/// Lock이 걸려있는 큐
/// </summary>
template<typename T>
class LockQueue
{
public:
	void Push(T item)
	{
		WRITE_LOCK;
		_items.push(item);
	}

	T Pop()
	{
		WRITE_LOCK;
		if (_items.empty())
			return T();

		T ret = _items.front();
		_items.pop();
		return ret;
	}

	void PopAll(OUT std::vector<T>& items)
	{
		std::queue<T> q;
		{
			WRITE_LOCK;
			q.swap(_items);
		}

		while (q.empty() == false)
		{
			items.push_back(q.front());
			q.pop();
		}
	}

	void PopCount(OUT std::vector<T>& items, const int32& count)
	{
		for (int32 i = 0; i < count; i++)
		{
			if (T item = Pop())
				items.push_back(item);
		}
	}

	void Clear()
	{
		WRITE_LOCK;
		_items = std::queue<T>();
	}

private:
	USE_LOCK;
	std::queue<T> _items;
};