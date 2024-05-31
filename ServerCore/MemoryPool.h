#pragma once
#include "Singleton.h"

constexpr int PoolSize = 4096;

template<class T, class... Args>
class MemoryPool
{
private:
	MemoryPool() = default;
	~MemoryPool()
	{
		WRITE_LOCK;
		while (_pools.empty() == false)
		{
			T* memory = _pools.top();
			if (memory != nullptr)
				delete memory;
			_pools.pop();
		}

		_pools.clear();
	}

public:
	static T* New(Args&&... args)
	{
		WRITE_LOCK;
		if (_pools.empty())
			return new T();

		T* memory = _pools.top();
		_pools.pop();

		return memory;
	}

	static void Delete(T* memory)
	{
		if (memory == nullptr)
			return;

		WRITE_LOCK;
		memory->Reset();
		_pools.push(memory);
	}

private:
	USE_LOCK;
	std::stack<T> _pools;
};

template<class Type, class... Args>
Type* xnew(Args&&... args)
{
	return MemoryPool::New(std::forward<Args>(args)...);
}

template<class Type>
void xdelete(Type* ptr)
{
	MemoryPool::Delete(ptr);
}

template<class T, class... Args>
std::shared_ptr<T> PoolAlloc(Args&&... args)
{
	return std::shared_ptr<T>{ xnew<T>(std::forward<Args>(args)...), xdelete<T> };
}