#include "pch.h"
#include "Socket.h"

NetAddress::NetAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr)
{

}

NetAddress::NetAddress(std::wstring ip, uint16 port)
{
	::memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr = Ip2Address(ip.c_str());
	_sockAddr.sin_port = ::htons(port);
}

std::wstring NetAddress::GetIpAddress()
{
	WCHAR buffer[100];
	int32 len = static_cast<int32>(sizeof(buffer) / sizeof(buffer[0]));
	::InetNtopW(AF_INET, &_sockAddr.sin_addr, buffer, len);
	return std::wstring(buffer);
}

IN_ADDR NetAddress::Ip2Address(const WCHAR* ip)
{
	IN_ADDR address;
	::InetPtonW(AF_INET, ip, &address);
	return address;
}

LPFN_CONNECTEX Socket::ConnectEx;
LPFN_DISCONNECTEX Socket::DisconnectEx;
LPFN_ACCEPTEX Socket::AcceptEx;
RIO_EXTENSION_FUNCTION_TABLE Socket::RIOEFTable;

void Socket::Init()
{
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == S_OK);

	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
#ifdef USE_RIO
	GUID guid = WSAID_MULTIPLE_RIO;
	DWORD bytes = 0;
	ASSERT_CRASH(::WSAIoctl(dummySocket, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), (void**)&RIOEFTable, sizeof(RIOEFTable), OUT &bytes, NULL, NULL) != SOCKET_ERROR);
#endif
	Close(dummySocket);
}

void Socket::Clear()
{
	::WSACleanup();
}

bool Socket::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;
	return ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL) != SOCKET_ERROR;
}

SOCKET Socket::CreateSocket()
{
#ifdef USE_RIO
	return ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_REGISTERED_IO);
#else
	return ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
#endif
}

bool Socket::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSocketOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool Socket::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool Socket::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool Socket::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool Socket::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSocketOpt(socket, IPPROTO_TCP, TCP_NODELAY, flag);
}

bool Socket::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool Socket::Bind(SOCKET socket, NetAddress netAddr)
{
	return ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN)) != SOCKET_ERROR;
}

bool Socket::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(SOCKADDR_IN)) != SOCKET_ERROR;
}

bool Socket::Listen(SOCKET socket, int32 backlog /*= SOMAXCONN*/)
{
	return ::listen(socket, backlog) != SOCKET_ERROR;
}

void Socket::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;
}
