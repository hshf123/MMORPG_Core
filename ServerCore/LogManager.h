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

class LogManager : public RefSingleton<LogManager>
{
public:
	LogManager();
	~LogManager();

public:
	void Initialize(const std::string& loggerName = "basic_logger", const std::string& filepath = "logs/basic-log.txt");
	void Launch();

	template <class... Args>
	void Log(const LogType& type, const bool& view, const bool& write, const std::string& fmt, Args&&... args);

private:
	void Write(const LogType& type, const std::string& log);
	void View(const LogType& type, const std::string& log);

public:
	USE_LOCKS(LogMax);
	std::shared_ptr<spdlog::logger> _logger = nullptr;
	std::queue<std::pair<LogType, std::string>> _logs[LogMax];
};

template <class... Args>
void LogManager::Log(const LogType& type, const bool& view, const bool& write, const std::string& fmt, Args&&... args)
{
	std::string log = std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));

	if (write)
	{
		WRITE_LOCKS(FileLog);
		_logs[FileLog].push({ type, log });
	}
	if (view)
	{
		WRITE_LOCKS(ConsoleLog);
		_logs[ConsoleLog].push({ type, log });
	}
}
