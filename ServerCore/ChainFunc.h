#pragma once
#include "Singleton.h"

template<class T>
class ChainFunc
{
public:
	ChainFunc& Chain(std::function<void(std::shared_ptr<T>)>&& fn);

	void Return(std::shared_ptr<T>&& value)
	{
		_returnValue = value;
	}

	std::shared_ptr<T> get() { return _returnValue; }

private:
	std::shared_ptr<T> _returnValue;
};

template<class T>
inline ChainFunc<T>& ChainFunc<T>::Chain(std::function<void(std::shared_ptr<T>)>&& fn)
{
	fn(_returnValue);
	return *this;
}

class ChainFuncMaker : public RefSingleton<ChainFuncMaker>
{
public:
	template<class T>
	ChainFunc<T> Make(std::function<T(void)>&& fn);
};

template<class T>
inline ChainFunc<T> ChainFuncMaker::Make(std::function<T(void)>&& fn)
{
	std::shared_ptr<T> ret = PoolAlloc<T>();
	*ret = fn();
	ChainFunc<T> cf;
	cf.Return(std::move(ret));
	return cf;
}
