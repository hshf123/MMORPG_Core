#include "pch.h"
#include "GameServer.h"

int main()
{
	GameServer server;
	if (server.Init() == false)
		return 0;

	server.Update();
	return 0;
}