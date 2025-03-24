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

void Monitor::PoolSizeCheck(const char* c, int32 poolSize, int32 useCount)
{
	if (100'000 > useCount)
		return;

	std::string className = StrUtils::ToString(c);
	VIEW_WARNING("{} PoolSize({}) UseCount({})", className, poolSize, useCount);
}
