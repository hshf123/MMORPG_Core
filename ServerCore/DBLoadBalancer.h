#pragma once
#include "DBService.h"

constexpr uint64 DBRECONNECTTIME = UINT64_C(5000);	// DB 재연결 시간

class DBLoadBalancer
{
public:
	/// <summary>
	/// DB 연결
	/// </summary>
	/// <param name="connectionString">연결을 위한 스트링, 아이디 비밀번호가 필요한 경우 따로 하나 더 만들어야함</param>
	/// <param name="serviceCount">DB 스레드 개수</param>
	/// <returns></returns>
	bool Init(const std::string& connectionString, const int32& serviceCount);
	/// <summary>
	/// 로드밸런싱할 DB 작업 Push
	/// </summary>
	/// <param name="workId">밸런싱할 ID</param>
	/// <param name="protocolId">프로토콜</param>
	/// <param name="data">DB스레드에 넘길 데이터, DBData를 상속 받은 데이터를 사용</param>
	/// <param name="handler">작업을 수행할 핸들러</param>
	/// <returns></returns>
	bool Push(const int32& workId, const uint16& protocolId, std::shared_ptr<DBData> data, DBHandler& handler);
	bool Push(std::shared_ptr<DBData> data, DBHandler& handler);

	void Launch();

private:
	std::string _connectionString = "";
	DBService*	_serviceList = nullptr;	// 배열로 사용
	int32		_serviceCount = 0;
};

