#pragma once

class DBData
{
public:
	DBData(uint16 protocolId, int32 workId = 0) : ProtocolID(protocolId), WorkID(workId) {}

	int32 WorkID = 0;		// �뷱�� �� ID
	uint16 ProtocolID = 0;	// ���� �� ��������
};

class DBService;
using DBHandlerFunc = std::function<bool(std::shared_ptr<DBData>, DBService*)>;
bool Handle_INVALID(std::shared_ptr<DBData> data, DBService* service);

class DBHandler
{
public:
	virtual void Init();
	/// <summary>
	/// DB �۾� ����
	/// </summary>
	/// <param name="protocolId">���� �� DB �������� ID</param>
	/// <param name="data">DB call�� �ʿ��� ������</param>
	/// <param name="service">�۾��� ������ DB ������</param>
	/// <returns></returns>
	bool HandleData(uint16 protocolId, std::shared_ptr<DBData> data, DBService* service);
	/// <summary>
	/// �ڵ鷯 ���
	/// </summary>
	/// <param name="protocol">���� �� ��������</param>
	/// <param name="fn">�������� ID�� �ش��ϴ� �ڵ鷯 �Լ� ������</param>
	/// <returns></returns>
	bool RegisterHandler(const uint16& protocol, DBHandlerFunc fn);
	template<class HandlerType, typename = typename std::enable_if<std::is_base_of<DBHandler, HandlerType>::value>::type>
	bool RegisterHandler(const uint16& protocol, bool (HandlerType::* handler)(std::shared_ptr<DBData>, DBService*))
	{
		return RegisterHandler(protocol, [=](std::shared_ptr<DBData> data, DBService* service)
			{
				return (static_cast<HandlerType*>(this)->*handler)(data, service);
			});
	}

private:
	DBHandlerFunc _dbHandler[UINT16_MAX];		// �ڵ鷯 ����Ʈ
#ifdef DEV_TEST
	std::unordered_set<uint16> _useProtocol;	// �ߺ� üũ��
#endif
};

