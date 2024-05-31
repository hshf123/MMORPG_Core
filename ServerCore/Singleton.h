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
	static T* GetInstance()
	{
		static T* instance;
		if (instance == nullptr)
			instance = new T();

		return instance;
	}
};