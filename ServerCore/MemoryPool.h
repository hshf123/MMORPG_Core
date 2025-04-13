#pragma once
#include "Singleton.h"
#include "LockFreeStack.h"
#include "Monitor.h"
#include "LogManager.h"
#include "TimeUtils.h"

#define USE_LOCK_FREE

template<class T, class... Args>
class MemoryPool : public RefSingleton<MemoryPool<T>>
{
public:
	MemoryPool() = default;
	~MemoryPool()
	{
#ifdef USE_LOCK_FREE
#else
		std::lock_guard<std::mutex> lock(_memoryLock);
		while (_pools.empty() == false)
		{
			void* memory = _pools.top();
			if (memory != nullptr)
				::free(memory);
			_pools.pop();
		}
#endif
	}

	void* New(Args&&... args)
	{
		_useSize.fetch_add(1);
#ifdef USE_LOCK_FREE
		T* memory = nullptr;
		_pools.TryPop(memory);
		if(memory == nullptr)
			return ::malloc(sizeof(T));
		_poolSize.fetch_add(-1);
		return memory;
#else
		std::lock_guard<std::mutex> lock(_memoryLock);
		if (_pools.empty())
			return ::malloc(sizeof(T));
		void* memory = _pools.top();
		_pools.pop();
		return memory;
#endif		
	}

	void Delete(T* memory)
	{
		if (memory == nullptr)
			return;
		_useSize.fetch_add(-1);
#ifdef USE_LOCK_FREE
		_pools.Push(memory);
		_poolSize.fetch_add(1);
#else
		std::lock_guard<std::mutex> lock(_memoryLock);
		_pools.push(memory);
#endif		
	}

	int32 GetPoolSize()
	{
#ifdef USE_LOCK_FREE
		return _poolSize.load();
#else
		return static_cast<int32>(_pools.size());
#endif
	}

	int32 GetUsingCount()
	{
		return _useSize.load();
	}

private:
	std::atomic<int32> _useSize = 0;
#ifdef USE_LOCK_FREE
	LockFreeStack<T> _pools;
	std::atomic<int32> _poolSize = 0;
#else
	std::mutex _memoryLock;
	std::stack<void*> _pools;
#endif
};

template<class Type, class... Args>
Type* xnew(Args&&... args)
{
	Type* mem = static_cast<Type*>(MemoryPool<Type>::GetInstance().New());
	new(mem)Type(std::forward<Args>(args)...);
	Monitor::GetInstance().PoolSizeCheck(typeid(Type).name(), MemoryPool<Type>::GetInstance().GetPoolSize(), MemoryPool<Type>::GetInstance().GetUsingCount());
	return mem;
}

template<class Type>
void xdelete(Type* ptr)
{
	ptr->~Type();
	MemoryPool<Type>::GetInstance().Delete(ptr);
}

template<class Type>
void xreserve(int32 size)
{
	std::vector<Type*> vec;
	vec.reserve(size);
	for (int32 i = 0; i < size; i++)
	{
		Type* mem = static_cast<Type*>(MemoryPool<Type>::GetInstance().New());
		new(mem)Type();
		vec.push_back(mem);
	}
	for (int32 i = 0; i < size; i++)
		xdelete(vec[i]);
}

template<class Type>
void log_poolsize()
{
	VIEW_WARNING("{} PoolSize({}) UseCount({})", typeid(Type).name(), MemoryPool<Type>::GetInstance().GetPoolSize(), MemoryPool<Type>::GetInstance().GetUsingCount());
}

/// <summary>
/// 메모리 풀을 사용한 smartpointer 반환
/// 풀에서 꺼내올 때 넣을 때 생성자 소멸자를 호출하기 때문에 별도의 Reset과 같은 함수를 만들 필요 없음.
/// 메모리 할당 방식은 new/delete, 기본 메모리 할당 방식을 사용함
/// </summary>
template<class T, class... Args>
std::shared_ptr<T> PoolAlloc(Args&&... args)
{
	return std::shared_ptr<T>{ xnew<T>(std::forward<Args>(args)...), xdelete<T> };
}