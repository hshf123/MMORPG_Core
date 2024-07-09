#include "pch.h"
#include "LogManager.h"
#include "ThreadManager.h"

LogManager::LogManager()
{

}

LogManager::~LogManager()
{
	std::queue<LogStruct> logQueue;
	{
		WRITE_LOCKS(ConsoleLog);
		_logs[ConsoleLog].swap(logQueue);
	}
	while (logQueue.empty() == false)
	{
		LogStruct p = logQueue.front();
		View(p.time, p.lv, p.log);
		logQueue.pop();
	}
	{
		WRITE_LOCKS(FileLog);
		_logs[FileLog].swap(logQueue);
	}
	while (logQueue.empty() == false)
	{
		LogStruct p = logQueue.front();
		Write(p.time, p.lv, p.log);
		logQueue.pop();
	}
}

void LogManager::Initialize(const std::string& loggerName /*= "basic_logger"*/, const std::string& filepath /*= "../logs/"*/)
{
	try
	{
		Poco::DateTime now = TimeUtils::GetPocoTime();
		std::string formattedDate = Poco::DateTimeFormatter::format(now, "%Y-%m-%d") + ".txt";
		_logger = spdlog::basic_logger_mt(loggerName, filepath + formattedDate);
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log init failed: " << ex.what() << std::endl;
	}
}

void LogManager::Launch()
{
	ThreadManager::GetInstance().Launch([&]()
		{
			while (true)
			{
				std::queue<LogStruct> logQueue;
				{
					WRITE_LOCKS(ConsoleLog);
					_directLogs[ConsoleLog].swap(logQueue);
				}
				while (logQueue.empty() == false)
				{
					const auto& log = logQueue.front();
					View(log.time, log.lv, log.log);
					logQueue.pop();
				}
				{
					WRITE_LOCKS(FileLog);
					_directLogs[FileLog].swap(logQueue);
				}
				while (logQueue.empty() == false)
				{
					const auto& log = logQueue.front();
					Write(log.time, log.lv, log.log);
					logQueue.pop();
				}
				{
					WRITE_LOCKS(ConsoleLog);
					_logs[ConsoleLog].swap(logQueue);
				}
				while (logQueue.empty() == false)
				{
					const auto& log = logQueue.front();
					View(log.time, log.lv, log.log);
					logQueue.pop();
				}
				{
					WRITE_LOCKS(FileLog);
					_logs[FileLog].swap(logQueue);
				}
				while (logQueue.empty() == false)
				{
					const auto& log = logQueue.front();
					Write(log.time, log.lv, log.log);
					logQueue.pop();
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		});
}

void LogManager::Write(const uint64& time, const LogType& type, const std::string_view& log)
{
	Log(time, type, log, _logger.get());
}

void LogManager::View(const uint64& time, const LogType& type, const std::string_view& log)
{
	Log(time, type, log);
}

void LogManager::Log(const uint64& time, const LogType& type, const std::string_view& log, spdlog::logger* logger /*= nullptr*/)
{
	if (logger == nullptr)
		logger = spdlog::default_logger_raw();

	spdlog::level::level_enum lvl = spdlog::level::off;
	switch (type)
	{
	case LogType::Info:
		lvl = spdlog::level::info;
		break;
	case LogType::Warning:
		lvl = spdlog::level::warn;
		break;
	case LogType::Error:
		lvl = spdlog::level::err;
		break;
	default:
		break;
	}

	auto local_time_point = std::chrono::system_clock::time_point(std::chrono::system_clock::duration(time));
	logger->log(local_time_point, spdlog::source_loc{}, lvl, log);
	if (lvl == spdlog::level::err)
		logger->flush();
}
