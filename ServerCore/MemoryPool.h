#pragma once
#include "Singleton.h"

constexpr int PoolSize = 4096;

template<class T, class... Args>
class MemoryPool : public RefSingleton<MemoryPool<T>>
{
public:
	MemoryPool() = default;
	~MemoryPool()
	{
		std::lock_guard<std::mutex> lock(_memoryLock);
		while (_pools.empty() == false)
		{
			void* memory = _pools.top();
			if (memory != nullptr)
				::free(memory);
			_pools.pop();
		}
	}

	void* New(Args&&... args)
	{
		std::lock_guard<std::mutex> lock(_memoryLock);
		if (_pools.empty())
			return ::malloc(sizeof(T));
		void* memory = _pools.top();
		_pools.pop();
		return memory;
	}

	void Delete(T* memory)
	{
		if (memory == nullptr)
			return;

		std::lock_guard<std::mutex> lock(_memoryLock);
		_pools.push(memory);
	}

private:
	std::mutex _memoryLock;
	std::stack<void*> _pools;
};

template<class Type, class... Args>
Type* xnew(Args&&... args)
{
	Type* mem = static_cast<Type*>(MemoryPool<Type>::GetInstance().New());
	new(mem)Type(std::forward<Args>(args)...);
	return mem;
}

template<class Type>
void xdelete(Type* ptr)
{
	ptr->~Type();
	MemoryPool<Type>::GetInstance().Delete(ptr);
}

template<class T, class... Args>
std::shared_ptr<T> PoolAlloc(Args&&... args)
{
	return std::shared_ptr<T>{ xnew<T>(std::forward<Args>(args)...), xdelete<T> };
}