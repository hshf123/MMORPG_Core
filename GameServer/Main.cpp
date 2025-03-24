#include "pch.h"
#include "GameServer.h"

//rapidjson 사용예제
//const std::string json = "{\"project\":\"rapidjson\",\"stars\":10}";
//rapidjson::Document d;
//d.Parse(json.c_str());
//rapidjson::Value& s = d["stars"];
//s.SetInt(s.GetInt() + 1);
//rapidjson::StringBuffer buffer;
//rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//d.Accept(writer);

int main()
{
	GameServer server;

	if (server.Init() == false)
		return 0;

	server.Update();
	return 0;
}