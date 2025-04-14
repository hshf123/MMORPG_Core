#include "pch.h"
#include "Monitor.h"

TimeMonitor::TimeMonitor(const char* fn)
{
	_tick = TimeUtils::GetTick64();
	_fn = StrUtils::ToString(fn);
}

TimeMonitor::~TimeMonitor()
{
	_tick = TimeUtils::GetTick64() - _tick;
	if (_tick < 100)
		return;
	VIEW_WRITE_WARNING("ThreadID({}) Execution time of function {}: {}ms", LThreadId, _fn, _tick);
}

Monitor::Monitor()
{
	::PdhOpenQuery(NULL, NULL, &_cpuQuery);
	::PdhAddCounterW(_cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &_cpuTotal);
}

const double Monitor::GetCPUUsage()
{
	::PdhCollectQueryData(_cpuQuery);
	::PdhGetFormattedCounterValue(_cpuTotal, PDH_FMT_DOUBLE, NULL, &_counterVal);
	return _counterVal.doubleValue;
}

const uint64 Monitor::GetMemoryUsage_Byte()
{
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (::GetProcessMemoryInfo(::GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
		return pmc.PrivateUsage;
	return 0;
}

const double Monitor::GetMemoryUsage_KB()
{
	return static_cast<double>(GetMemoryUsage_Byte()) / 1024.0;
}

const double Monitor::GetMemoryUsage_MB()
{
	return GetMemoryUsage_KB() / 1024.0;
}

void Monitor::PrintServerCounting()
{
	int64_t now = TimeUtils::GetTick64();
	// 최소 1분은 수집
	if (_lastCountingTick + (1000 * 60) > now)
		return;

	VIEW_WRITE_INFO("Accept({}) Recv({} / {}) Send({} / {}) Process({} / {})", _acceptCount.load(), _recvCount.load(), _recvSize.load(), _sendCount.load(), _sendSize.load(), _processCount.load(), _processingCount.load());
	_acceptCount = 0;
	_sendCount = 0;
	_sendSize = 0;
	_recvCount = 0;
	_recvSize = 0;
	_processCount = 0;
	_lastCountingTick = now;
}

void Monitor::AddAcceptCount(int32 acceptCount)
{
	_acceptCount.fetch_add(acceptCount);
}

void Monitor::AddRecvCount(int32 recvCount, int32 size /*= 0*/)
{
	_recvCount.fetch_add(recvCount);
	_recvSize.fetch_add(size);
}

void Monitor::AddSendCount(int32 sendCount, int32 size /*= 0*/)
{
	_sendCount.fetch_add(sendCount);
	_sendSize.fetch_add(size);
}

void Monitor::IncProcessCount()
{
	_processingCount.fetch_add(1);
}

void Monitor::DecProcessCount()
{
	_processingCount.fetch_add(-1);
}

void Monitor::AddProcessCount()
{
	_processCount.fetch_add(1);
}

void Monitor::PoolSizeCheck(const char* c, int32 poolSize, int32 useCount)
{
	/*if (100'000 > useCount || poolSize != 0)
		return;

	std::string className = StrUtils::ToString(c);
	VIEW_WARNING("{} PoolSize({}) UseCount({})", className, poolSize, useCount);*/
}
