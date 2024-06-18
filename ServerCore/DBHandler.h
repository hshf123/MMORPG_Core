#pragma once

class DBData
{
public:
	uint16 ProtocolID = 0;
	int32 WorkID = 0;
};

using DBHandlerFunc = std::function<bool(std::shared_ptr<DBData>)>;
bool Handle_INVALID(std::shared_ptr<DBData>);

class DBHandler
{
public:
	void Init();
	bool HandlePacket(uint16 protocolId, std::shared_ptr<DBData> data);
	bool RegisterHandler(const uint16& protocol, DBHandlerFunc fn);

private:
	template<class ProcessFunc>
	bool HandlePacket(ProcessFunc func, std::shared_ptr<DBData> data)
	{
		return func(data);
	}

private:
	DBHandlerFunc _dbHandler[UINT16_MAX];
#ifdef DEV_TEST
	std::unordered_set<uint16> _useProtocol;
#endif
};

