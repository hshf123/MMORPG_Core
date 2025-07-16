#pragma once
#include "Singleton.h"
#include "JobQueue.h"

template<class T>
class ChainFunc : public JobQueue
{
public:
	~ChainFunc() { VIEW_INFO("DELETE CHAIN"); }

	template <class F, std::enable_if_t<std::is_same_v<void, std::invoke_result_t<F, T>>, int> = true>
	std::shared_ptr<ChainFunc<T>> Chain(F&& fn)
	{
		DoAsync([=]()
			{
				fn(*this->_returnValue);
			});
		return static_pointer_cast<ChainFunc<T>>(shared_from_this());
	}
	template <class F, std::enable_if_t<!std::is_same_v<void, std::invoke_result_t<F, T>>, int> = true>
	std::shared_ptr<ChainFunc<T>> Chain(F&& fn)
	{
		DoAsync([=]()
			{
				*this->_returnValue = fn(*this->_returnValue);
			});
		return static_pointer_cast<ChainFunc<T>>(shared_from_this());
	}
	template <class F, std::enable_if_t<std::is_same_v<void, std::invoke_result_t<F, T>>, int> = true>
	std::shared_ptr<ChainFunc<T>> ChainAsync(F&& fn)
	{
		DoAsyncToss([=]()
			{
				fn(*this->_returnValue);
			});
		return static_pointer_cast<ChainFunc<T>>(shared_from_this());
	}
	template <class F, std::enable_if_t<!std::is_same_v<void, std::invoke_result_t<F, T>>, int> = true>
	std::shared_ptr<ChainFunc<T>> ChainAsync(F&& fn)
	{
		DoAsyncToss([=]()
			{
				*this->_returnValue = fn(*this->_returnValue);
			});
		return static_pointer_cast<ChainFunc<T>>(shared_from_this());
	}

	void Return(std::shared_ptr<T>&& value) { _returnValue = value; }

private:
	std::shared_ptr<T> _returnValue;
};

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
