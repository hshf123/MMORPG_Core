#pragma once

// for, RIO
// 처음 256개 설정 클라 연결 수가 많아져서 에코 패킷 량이 많아지면 당연히 I/O 작업도 늘어날텐데
// 평균적으로 30~50개에 머무르다 약간 딜레이가 생겨서 300~400개가 생기는 경우가 종종 있음 -> 이 때 밀려서 10055에러 내뱉는 듯
// PC 사양에 따라서 다른지 노트북은 생기는데 데탑은 밀리는 경우가 없긴 함
// 512는 좀 큰 것 같고 256개에서 I/O가 좀 밀린다 싶으면 늘릴 수 있도록 const로 말고 따로 빼서 Throughput조절용으로 빼는 것도 괜찮을 듯
constexpr int RIO_DISPATCH_RESULT_COUNT = 256;	// Dispatch 1번으로 가져올 최대 결과 개수

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