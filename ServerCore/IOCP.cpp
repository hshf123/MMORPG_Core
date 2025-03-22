#include "pch.h"
#include "IOCP.h"
#include "Service.h"
#include "Monitor.h"

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(std::shared_ptr<IocpObject> iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs /*= INFINITE*/)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT & key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		TimeMonitor tm(__FUNCTION__);
#ifdef USE_RIO
		if (key == RIO_IOCP_COMPLETION)
		{
			std::shared_ptr<Service> service = iocpEvent->ownerService;
			service->Dispatch(static_cast<RIONotifyEvent*>(iocpEvent));
			return true;
		}
#endif
		std::shared_ptr<IocpObject> iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errorCode = ::WSAGetLastError();
		switch (errorCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
		{
#ifdef USE_RIO
			if (key == RIO_IOCP_COMPLETION)
			{
				std::shared_ptr<Service> service = iocpEvent->ownerService;
				service->Dispatch(static_cast<RIONotifyEvent*>(iocpEvent));
				return true;
			}
#endif
			std::shared_ptr<IocpObject> iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
		}
		break;
		}
	}

	return true;
}

IocpEvent::IocpEvent(EventType type) : eventType(type)
{
	Init();
}

void IocpEvent::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}

void RIOEvent::Init()
{
	BufferId = 0;
	Offset = 0;
	Length = 0;
}
