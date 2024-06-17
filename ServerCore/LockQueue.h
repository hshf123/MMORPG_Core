#pragma once

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
		WRITE_LOCK;
		while (T item = Pop())
			items.push_back(item);
	}

	void PopCount(OUT std::vector<T>& items, const int32& count)
	{
		WRITE_LOCK;
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