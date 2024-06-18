#pragma once

class DBData
{
public:
	DBData(uint16 protocolId, int32 workId) : ProtocolID(protocolId), WorkID(workId) {}

	uint16 ProtocolID = 0;
	int32 WorkID = 0;
};

class DBService;
using DBHandlerFunc = std::function<bool(std::shared_ptr<DBData>, DBService*)>;
bool Handle_INVALID(std::shared_ptr<DBData> data, DBService* service);

class DBHandler
{
public:
	virtual void Init();
	bool HandlePacket(uint16 protocolId, std::shared_ptr<DBData> data, DBService* service);
	bool RegisterHandler(const uint16& protocol, DBHandlerFunc fn);
	template<class HandlerType, typename = typename std::enable_if<std::is_base_of<DBHandler, HandlerType>::value>::type>
	bool RegisterHandler(const uint16& protocol, bool (HandlerType::* handler)(std::shared_ptr<DBData>, DBService*))
	{
		return RegisterHandler(protocol, [=](std::shared_ptr<DBData> data, DBService* service)
			{
				return (static_cast<HandlerType*>(this)->*handler)(data, service);
			});
	}

protected:
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

