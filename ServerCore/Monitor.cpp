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
	if (_tick == 0)
		return;
	Monitor::GetInstance().TimeMonitorCheck(_fn, _tick);
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
	PROCESS_MEMORY_COUNTERS pmc;
	if (::GetProcessMemoryInfo(::GetCurrentProcess(), &pmc, sizeof(pmc)))
		return pmc.WorkingSetSize;
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

void Monitor::TimeMonitorCheck(const std::string& fn, uint64 tick)
{
	auto findIt = _timeMonitorList.find(fn);
	if (findIt == _timeMonitorList.end() || findIt->second.first < tick)
		_timeMonitorList.insert_or_assign(fn, std::make_pair(tick, TimeUtils::GetTick64()));
}

void Monitor::PrintTimeMonitorList(uint64 tick)
{
	for (const auto& [fn, p] : _timeMonitorList)
	{
		if (p.first < tick)
			continue;
		std::string formattedTime = Poco::DateTimeFormatter::format(TimeUtils::TickToPocoTime(p.second), "%Y-%m-%d %H:%M:%S");
		VIEW_INFO("Function({}) ExecutionTick([{}] ms) LastCheckTime({}) ThreadID({})", fn, p.first, formattedTime, LThreadId);
	}
	_timeMonitorList.clear();
}

void Monitor::PoolSizeCheck(const char* c, int32 poolSize, int32 useCount)
{
	if (100 > useCount)
		return;

	std::string className = StrUtils::ToString(c);
	VIEW_WARNING("{} PoolSize({}) UseCount({})", className, poolSize, useCount);
}
