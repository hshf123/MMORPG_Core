#pragma once
#include "DBService.h"

constexpr uint64 DBRECONNECTTIME = UINT64_C(5000);	// DB �翬�� �ð�

class DBLoadBalancer
{
public:
	/// <summary>
	/// DB ����
	/// </summary>
	/// <param name="connectionString">������ ���� ��Ʈ��, ���̵� ��й�ȣ�� �ʿ��� ��� ���� �ϳ� �� ��������</param>
	/// <param name="serviceCount">DB ������ ����</param>
	/// <returns></returns>
	bool Init(const std::string& connectionString, const int32& serviceCount);
	/// <summary>
	/// �ε�뷱���� DB �۾� Push
	/// </summary>
	/// <param name="workId">�뷱���� ID</param>
	/// <param name="protocolId">��������</param>
	/// <param name="data">DB�����忡 �ѱ� ������, DBData�� ��� ���� �����͸� ���</param>
	/// <param name="handler">�۾��� ������ �ڵ鷯</param>
	/// <returns></returns>
	bool Push(const int32& workId, const uint16& protocolId, std::shared_ptr<DBData> data, DBHandler& handler);
	bool Push(std::shared_ptr<DBData> data, DBHandler& handler);

	void Launch();

private:
	std::string _connectionString = "";
	DBService*	_serviceList = nullptr;	// �迭�� ���
	int32		_serviceCount = 0;
};

