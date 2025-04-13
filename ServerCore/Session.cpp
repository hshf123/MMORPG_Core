#include "pch.h"
#include "Session.h"
#include "Socket.h"
#include "Service.h"
#include "SendBuffer.h"

Session::Session() : _recvBuffer(RECV_BUFFER_SIZE), _sendBuffer(SEND_BUFFER_SIZE)
{
	_socket = Socket::CreateSocket();
}

Session::~Session()
{
#ifdef USE_RIO
	Socket::RIOEFTable.RIODeregisterBuffer(_rioRecvBufferId);
	Socket::RIOEFTable.RIODeregisterBuffer(_rioSendBufferId);
#endif
	Socket::Close(_socket);
	{
		std::queue<std::shared_ptr<SendBuffer>> tq;
		WRITE_LOCKS(SEND_QUEUE_LOCK);
		tq.swap(_sendQueue);
	}
	_sendRegistered.store(false);
}

void Session::Send(std::shared_ptr<SendBuffer> buffer)
{
	if (IsConnected() == false)
		return;

	bool registerSend = false;

	{
		WRITE_LOCKS(SEND_QUEUE_LOCK);

		_sendQueue.push(buffer);

		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;

	VIEW_WRITE_WARNING("Disconnected {}", StrUtils::ToString(cause));
	RegisterDisconnect();
}

bool Session::RegisterRIOBuffer()
{
	_rioRecvBufferId = Socket::RIOEFTable.RIORegisterBuffer(reinterpret_cast<PCHAR>(_recvBuffer.WritePos()), _recvBuffer.FreeSize());
	if (_rioRecvBufferId == RIO_INVALID_BUFFERID)
		return false;
	_rioSendBufferId = Socket::RIOEFTable.RIORegisterBuffer(reinterpret_cast<PCHAR>(_sendBuffer.WritePos()), _sendBuffer.FreeSize());
	if (_rioSendBufferId == RIO_INVALID_BUFFERID)
		return false;
	return true;
}

bool Session::CreateRIORQ()
{
	_rioRQ = Socket::RIOEFTable.RIOCreateRequestQueue(_socket
		, OUTSTANDING_COUNT					// Max OutStanding Recv
		, 1									// Recv Buffer Count Must be 1?
		, OUTSTANDING_COUNT					// Max OutStanding Send
		, 1									// Send Buffer Count Must be 1?
		, GetService()->GetRIOCQ(GetWorkId())
		, GetService()->GetRIOCQ(GetWorkId())
		, nullptr);
	if (_rioRQ == RIO_INVALID_RQ)
	{
		int32 errorCode = ::GetLastError();
		HandleError(errorCode);
		Disconnect(L"Request Queue Create Fail");
		return false;
	}

	return true;
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes /*= 0*/)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
#ifdef USE_RIO
#else
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		Monitor::GetInstance().AddRecvCount(1);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		Monitor::GetInstance().AddSendCount(1);
		break;
#endif
	default:
		break;
	}
}

void Session::Dispatch(RIOEvent* rioEvent, int32 numOfBytes /*= 0*/)
{
	switch (rioEvent->eventType)
	{	// RIO는 Send / Recv만 작동
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

void Session::SetWorkId()
{
	static std::atomic_int32_t SWorkId = 1;
	_workId = SWorkId.fetch_add(1);
}

bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (Socket::SetReuseAddress(_socket, true) == false)
		return false;

	if (Socket::BindAnyAddress(_socket, 0) == false)
		return false;

	_connectEvent.Init();
	_connectEvent.owner = shared_from_this();

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (Socket::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent) == false)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connectEvent.owner = nullptr;
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this(); // ADD_REF

	if (false == Socket::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_disconnectEvent.owner = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;
#ifdef USE_RIO
	_rioRecvEvent.Init();
	_rioRecvEvent.owner = shared_from_this();

	_rioRecvEvent.BufferId = _rioRecvBufferId;
	_rioRecvEvent.Length = _recvBuffer.FreeSize();
	_rioRecvEvent.Offset = static_cast<uint64>(_recvBuffer.WriteOffset());

	DWORD recvbytes = 0;
	DWORD flags = 0;

	WRITE_LOCKS(RIO_RQ_LOCK);
	if (Socket::RIOEFTable.RIOReceive(_rioRQ, static_cast<PRIO_BUF>(&_rioRecvEvent), 1, flags, &_rioRecvEvent) == false)
	{
		int32 errorCode = ::GetLastError();
		HandleError(errorCode);
		_rioRecvEvent.owner = nullptr;
	}
#else
	_recvEvent.Init();
	_recvEvent.owner = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &_recvEvent, nullptr) == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr;
		}
	}
#endif
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;
	
#ifdef USE_RIO
	std::queue<std::shared_ptr<SendBuffer>> sendQueue;
	{
		WRITE_LOCKS(SEND_QUEUE_LOCK);
		sendQueue.swap(_sendQueue);
	}
	_rioSendEvent.Init();
	_rioSendEvent.owner = shared_from_this();
	_rioSendEvent.BufferId = _rioSendBufferId;
	_rioSendEvent.Offset = _sendBuffer.WriteOffset();
	while (sendQueue.empty() == false)
	{
		std::shared_ptr<SendBuffer> sendBuffer = sendQueue.front();
		if (_sendBuffer.FreeSize() < sendBuffer->WriteSize())
		{
			VIEW_WRITE_ERROR("There's no FreeSize Session({})", _workId);
			Disconnect(L"SendBuffer Over");
			return;
		}

		sendQueue.pop();
		::memcpy_s(_sendBuffer.WritePos(), sendBuffer->WriteSize(), sendBuffer->Buffer(), sendBuffer->WriteSize());

		_rioSendEvent.Length += sendBuffer->WriteSize();
		_sendBuffer.OnWrite(sendBuffer->WriteSize());
	}
	DWORD sendbytes = 0;
	DWORD flags = 0;
	WRITE_LOCKS(RIO_RQ_LOCK);
	if (Socket::RIOEFTable.RIOSend(_rioRQ, &_rioSendEvent, 1, flags, &_rioSendEvent) == false)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_rioSendEvent.owner = nullptr;
			_sendRegistered.store(false);
		}
	}
#else
	_sendEvent.Init();
	_sendEvent.owner = shared_from_this();

	{
		WRITE_LOCKS(SEND_QUEUE_LOCK);

		int32 writeSize = 0;
		while (_sendQueue.empty() == false)
		{
			std::shared_ptr<SendBuffer> sendBuffer = _sendQueue.front();
			writeSize += sendBuffer->WriteSize();
			_sendQueue.pop();
			_sendEvent.SendBuffers.push_back(sendBuffer);
		}
	}

	std::vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.SendBuffers.size());
	for (std::shared_ptr<SendBuffer> sendBuffer : _sendEvent.SendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr) == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent.owner = nullptr;
			_sendEvent.SendBuffers.clear();
			_sendRegistered.store(false);
		}
	}
#endif
}

void Session::ProcessConnect()
{
	_connected.store(true);
	GetService()->AddSession(GetSession());
#ifdef USE_RIO
	RegisterRIOBuffer();
	CreateRIORQ();
#endif
	SetWorkId();
	OnConnected(GetService()->GetNetAddress());
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent.owner = nullptr;

	OnDisconnected();
	GetService()->ReleaseSession(GetSession());
}

void Session::ProcessRecv(int32 numOfBytes)
{
#ifdef USE_RIO
	_rioRecvEvent.owner = nullptr;
#else
	_recvEvent.owner = nullptr;
#endif
	if (numOfBytes == 0)
	{
		Disconnect(L"0 Bytes Recv");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	_recvBuffer.Clean();
	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
#ifdef USE_RIO
	_sendBuffer.OnRead(numOfBytes);
	_sendBuffer.Clean();
	_rioSendEvent.owner = nullptr;
#else
	_sendEvent.owner = nullptr;
	_sendEvent.SendBuffers.clear();
#endif
	if (numOfBytes == 0)
	{
		Disconnect(L"0 Bytes Send");
		return;
	}

	OnSend(numOfBytes);

	WRITE_LOCKS(SEND_QUEUE_LOCK);
	if (_sendQueue.size() >= WARN_SENDQUEUE_SIZE)
		VIEW_WRITE_WARNING("WorkId({}) SendQueueSize Over {}!! QueueSize({})", GetWorkId(), WARN_SENDQUEUE_SIZE, _sendQueue.size());
	if (_sendQueue.size() >= WARN_SENDQUEUE_SIZE)
	{
		VIEW_WRITE_WARNING("WorkId({}) Kick By SendQueueSize Over {}!! QueueSize({})", GetWorkId(), KICK_SENDQUEUE_SIZE, _sendQueue.size());
		Disconnect(L"SendQueue Size Over");
		return;
	}
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	case WSAEFAULT:
	case WSAEINVAL:
		Disconnect(L"INVALID REQ");	// 버퍼 해제, 잘못된 매개변수 전달 등... 그냥 재접 시켜
		break;
	case WSAENOBUFS:
		Disconnect(L"MEMORY FULL");	// I/O 완료 큐가 가득 찼을 경우, 클라가 느리게 처리하는 경우거나 보내는게 느린건데 어쨌든 쌓인게 많으니 그냥 재접 시키는게
		break;
	default:
		VIEW_WRITE_ERROR("Handle Error, {}", errorCode);
		break;
	}
}

PacketSession::PacketSession()
{

}

PacketSession::~PacketSession()
{

}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *reinterpret_cast<PacketHeader*>(&buffer[processLen]);
		if (dataSize < header.size)
			break;

		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}