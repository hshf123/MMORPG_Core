#pragma once

template<class T>
class RefSingleton
{
public:
	static T& GetInstance()
	{
		static T instance;
		return instance;
	}
};

template<class T>
class PtrSingleton
{
public:
	static std::shared_ptr<T> GetInstance()
	{
		static std::shared_ptr<T> instance;
		if (instance == nullptr)
			instance = std::make_shared<T>();

		return instance;
	}
};