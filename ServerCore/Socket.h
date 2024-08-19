#pragma once

// for, RIO
// ó�� 256�� ���� Ŭ�� ���� ���� �������� ���� ��Ŷ ���� �������� �翬�� I/O �۾��� �þ�ٵ�
// ��������� 30~50���� �ӹ����� �ణ �����̰� ���ܼ� 300~400���� ����� ��찡 ���� ���� -> �� �� �з��� 10055���� ����� ��
// PC ��翡 ���� �ٸ��� ��Ʈ���� ����µ� ��ž�� �и��� ��찡 ���� ��
// 512�� �� ū �� ���� 256������ I/O�� �� �и��� ������ �ø� �� �ֵ��� const�� ���� ���� ���� Throughput���������� ���� �͵� ������ ��
constexpr int RIO_DISPATCH_RESULT_COUNT = 256;	// Dispatch 1������ ������ �ִ� ��� ����

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(std::wstring ip, uint16 port);

	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	std::wstring GetIpAddress();
	uint16 GetPort() { return ::ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN _sockAddr = {};
};

class Socket
{
public:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;
	static RIO_EXTENSION_FUNCTION_TABLE RIOEFTable;

public:
	static void Init();
	static void Clear();

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static SOCKET CreateSocket();

	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, int32 size);
	static bool SetSendBufferSize(SOCKET socket, int32 size);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	static bool Bind(SOCKET socket, NetAddress netAddr);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static void Close(SOCKET& socket);
};

template<class T>
static inline bool SetSocketOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T)) != SOCKET_ERROR;
}