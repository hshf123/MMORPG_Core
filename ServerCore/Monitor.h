#pragma once
#include "Singleton.h"
#include "pdh.h"
#pragma comment(lib, "pdh.lib")
#include <psapi.h>

class TimeMonitor
{
public:
	TimeMonitor(const char* fn);
	~TimeMonitor();

private:
	uint64 _tick = INT64_C(0);
	std::string _fn = {};
};

class Monitor : public RefSingleton<Monitor>
{
public:
	Monitor();

	// CPU 사용량
	const double GetCPUUsage();
	// 메모리 사용량
	const uint64 GetMemoryUsage_Byte();
	const double GetMemoryUsage_KB();
	const double GetMemoryUsage_MB();

	// 함수 소요 시간 체크
	void TimeMonitorCheck(const std::string& fn, uint64 tick);
	// 일정 시간 이상 걸린 함수 출력
	void PrintTimeMonitorList(uint64 tick);

	void PoolSizeCheck(const char* c, int32 poolSize, int32 useCount);
private:
	PDH_HQUERY _cpuQuery = {};
	PDH_HCOUNTER _cpuTotal = {};
	PDH_FMT_COUNTERVALUE _counterVal = {};

	std::unordered_map<std::string, std::pair<uint64, uint64>> _timeMonitorList;
};

