#pragma once
#include "Singleton.h"
#include "JobQueue.h"

template<class T>
class ChainFunc : public JobQueue
{
public:
	~ChainFunc() { VIEW_INFO("DELETE CHAIN"); }

	std::shared_ptr<ChainFunc> Chain(std::function<void(std::shared_ptr<T>)>&& fn);
	std::shared_ptr<ChainFunc> ChainAsync(std::function<void(std::shared_ptr<T>)>&& fn);
	void Return(std::shared_ptr<T>&& value) { _returnValue = value; }


private:
	std::shared_ptr<T> _returnValue;
};

template<class T>
inline std::shared_ptr<ChainFunc<T>> ChainFunc<T>::Chain(std::function<void(std::shared_ptr<T>)>&& fn)
{
	DoAsync([=]()
		{
			fn(this->_returnValue);
		});
	return static_pointer_cast<ChainFunc<T>>(shared_from_this());
}

template<class T>
inline std::shared_ptr<ChainFunc<T>> ChainFunc<T>::ChainAsync(std::function<void(std::shared_ptr<T>)>&& fn)
{
	DoAsyncToss([=]()
		{
			fn(this->_returnValue);
		});
	return static_pointer_cast<ChainFunc<T>>(shared_from_this());
}

class ChainFuncMaker : public RefSingleton<ChainFuncMaker>
{
public:
	template<class T>
	std::shared_ptr<ChainFunc<T>> Make(std::function<T(void)>&& fn);
};

template<class T>
inline std::shared_ptr<ChainFunc<T>> ChainFuncMaker::Make(std::function<T(void)>&& fn)
{
	std::shared_ptr<ChainFunc<T>> chain = PoolAlloc<ChainFunc<T>>();
	chain->DoAsync([=]()
		{
			std::shared_ptr<T> ret = PoolAlloc<T>();
			*ret = fn();
			chain->Return(std::move(ret));
		});
	return chain;
}
