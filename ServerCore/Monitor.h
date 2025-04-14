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

	void PrintServerCounting();
	void AddAcceptCount(int32 acceptCount);
	void AddRecvCount(int32 recvCount, int32 size = 0);
	void AddSendCount(int32 sendCount, int32 size = 0);

	void AddProcessCount();

	void PoolSizeCheck(const char* c, int32 poolSize, int32 useCount);
private:
	PDH_HQUERY _cpuQuery = {};
	PDH_HCOUNTER _cpuTotal = {};
	PDH_FMT_COUNTERVALUE _counterVal = {};

	std::unordered_map<std::string, std::pair<uint64, uint64>> _timeMonitorList;

	int64_t _lastCountingTick = INT64_C(0);
	std::atomic_int32_t _acceptCount = 0;
	std::atomic_int64_t _recvCount = 0;
	std::atomic_int64_t _sendCount = 0;
	std::atomic_int64_t _recvSize = 0;
	std::atomic_int64_t _sendSize = 0;

	std::atomic_int32_t _processCount = 0;
};

