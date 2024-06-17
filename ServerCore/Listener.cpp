#include "pch.h"
#include "Listener.h"
#include "Socket.h"
#include "Session.h"
#include "Service.h"

Listener::~Listener()
{
	Socket::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{

		xdelete(acceptEvent);
	}
}

bool Listener::StartAccept(std::shared_ptr<ServerService> service)
{
	_service = service;
	if (_service == nullptr)
		return false;

	_socket = Socket::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	if (_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	if (Socket::SetReuseAddress(_socket, true) == false)
		return false;

	if (Socket::SetLinger(_socket, 0, 0) == false)
		return false;

	if (Socket::Bind(_socket, service->GetNetAddress()) == false)
		return false;

	if (Socket::Listen(_socket) == false)
		return false;

	const int32 acceptCount = _service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		acceptEvent->owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	Socket::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, [[maybe_unused]] int32 numOfBytes /*= 0*/)
{
	ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	std::shared_ptr<Session> session = _service->CreateSession();

	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD bytesReceived = 0;
	if (Socket::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)) == false)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	std::shared_ptr<Session> session = acceptEvent->session;

	if (Socket::SetUpdateAcceptSocket(session->GetSocket(), _socket) == false)
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	const int32 peerName = ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr);
	if (peerName == SOCKET_ERROR)
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddr(NetAddress(sockAddress));
	session->ProcessConnect();
	RegisterAccept(acceptEvent);
}
