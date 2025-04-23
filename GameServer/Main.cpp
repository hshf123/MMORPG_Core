#include "pch.h"
#include "GameServer.h"

int main()
{
	std::vector<redisContext*> vec;
	for (int i = 0; i < 10; i++)
	{
		redisContext* c = redisConnect("127.0.0.1", 6379);
		if (c == nullptr || c->err) {
			std::cerr << "Redis connection error: " << (c ? c->errstr : "Can't allocate context") << std::endl;
			return 1;
		}

		vec.push_back(c);
	}

	auto fn = [&](redisContext* c)
		{
			while (true)
			{
				redisReply* reply = (redisReply*)redisCommand(c, "SET game_key player1");
				std::cout << "SET: " << reply->str << std::endl;

				reply = (redisReply*)redisCommand(c, "GET game_key");
				std::cout << "GET: " << reply->str << std::endl;
				freeReplyObject(reply);

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		};
	
	std::vector<std::jthread> tvec;
	for (int i = 0; i < 10; i++)
	{
		std::jthread t(fn, vec[i]);
		tvec.push_back(std::move(t));
	}

	while (true);

	GameServer server;
	if (server.Init() == false)
	{
		std::cout << "Init Fail" << std::endl;
		return 0;
	}

	server.Update();
	return 0;
}