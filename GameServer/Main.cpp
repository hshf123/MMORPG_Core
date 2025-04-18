#include "pch.h"
#include "GameServer.h"

int main()
{
	GameServer server;
	if (server.Init() == false)
	{
		std::cout << "Init Fail" << std::endl;
		return 0;
	}

	server.Update();
	return 0;
}