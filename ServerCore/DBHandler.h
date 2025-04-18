#pragma once

class DBData
{
public:
	DBData();
	~DBData();

	int32 WorkID = 0;		// 밸런싱 할 ID
	uint16 ProtocolID = 0;	// 서버 내 프로토콜

	std::shared_ptr<class JobQueue> Owner = nullptr;
	std::shared_ptr<class Job> ResponseJob = nullptr;
	void ProcessDBWorking();
};

class DBService;
using DBHandlerFunc = std::function<bool(std::shared_ptr<DBData>)>;
bool Handle_INVALID(std::shared_ptr<DBData> data);

class DBHandler
{
public:
	virtual void Init();
	/// <summary>
	/// DB 작업 수행
	/// </summary>
	/// <param name="protocolId">서버 내 DB 프로토콜 ID</param>
	/// <param name="data">DB call에 필요한 데이터</param>
	/// <param name="service">작업을 수행할 DB 스레드</param>
	/// <returns></returns>
	bool HandleData(uint16 protocolId, std::shared_ptr<DBData> data);
	/// <summary>
	/// 핸들러 등록
	/// </summary>
	/// <param name="protocol">서버 내 프로토콜</param>
	/// <param name="fn">프토토콜 ID에 해당하는 핸들러 함수 포인터</param>
	/// <returns></returns>
	bool RegisterHandler(const uint16& protocol, DBHandlerFunc fn);
	template<class HandlerType, typename = typename std::enable_if<std::is_base_of<DBHandler, HandlerType>::value>::type>
	bool RegisterHandler(const uint16& protocol, bool (HandlerType::* handler)(std::shared_ptr<DBData>))
	{
		return RegisterHandler(protocol, [=](std::shared_ptr<DBData> data)
			{
				return (static_cast<HandlerType*>(this)->*handler)(data);
			});
	}

private:
	DBHandlerFunc _dbHandler[UINT16_MAX];		// 핸들러 리스트
#ifdef DEV_TEST
	std::unordered_set<uint16> _useProtocol;	// 중복 체크용
#endif
};

