#pragma once
#include "Singleton.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/common.h>

#define ConsoleLog		0
#define FileLog			1
#define LogMax			2

enum class LogType
{
	Info,
	Warning,
	Error
};

struct LogStruct
{
	LogStruct() = default;
	LogStruct(LogType lv, uint64 time, std::string_view log) : lv(lv), time(time), log(log) {}

	LogType lv = LogType::Info;
	uint64 time = UINT64_C(0);
	std::string log = "";
};

/// <summary>
/// spdlog 매핑 클래스
/// </summary>
class LogManager : public RefSingleton<LogManager>
{
public:
	LogManager();
	~LogManager();

public:
	void Initialize(const std::string& loggerName = "basic_logger", const std::string& filepath = "../logs/");
	void Launch();

	template <class... Args>
	void Log(const uint64& time, const LogType& type, const bool& view, const bool& write, const std::string& fmt, Args&&... args);

private:
	void Write(const uint64& time, const LogType& type, const std::string_view& log);
	void View(const uint64& time, const LogType& type, const std::string_view& log);
	void Log(const uint64& time, const LogType& type, const std::string_view& log, spdlog::logger* logger = nullptr);

private:
	USE_LOCKS(LogMax);
	std::shared_ptr<spdlog::logger> _logger = nullptr;
	std::queue<LogStruct> _logs[LogMax];
	std::queue<LogStruct> _directLogs[LogMax];
	std::chrono::seconds _offset = {};
};

template <class... Args>
void LogManager::Log(const uint64& time, const LogType& type, const bool& view, const bool& write, const std::string& fmt, Args&&... args)
{
	std::string log = std::vformat(fmt, std::make_format_args(args...));

	if (write)
	{
		WRITE_LOCKS(FileLog);
		if (type == LogType::Error)
			_directLogs[FileLog].push({ type, time, log });
		else
			_logs[FileLog].push({ type, time, log });
	}
	if (view)
	{
		WRITE_LOCKS(ConsoleLog);
		if (type == LogType::Error)
			_directLogs[ConsoleLog].push({ type, time, log });
		else
			_logs[ConsoleLog].push({ type, time, log });
	}
}
