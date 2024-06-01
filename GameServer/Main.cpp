#include "pch.h"
#include "CorePch.h"
#include "LogManager.h"

int main()
{
	LogManager::GetInstance().Initialize();


	LogManager::GetInstance().Launch();

	for (int i = 0; i < 100; i++)
		Log("Hello World {}", i);

	return 0;
}