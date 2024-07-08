#include "pch.h"
#include "LogManager.h"
#include "ThreadManager.h"

LogManager::LogManager()
{

}

LogManager::~LogManager()
{
	std::queue<std::pair<LogType, std::string>> logQueue;
	{
		WRITE_LOCKS(ConsoleLog);
		_logs[ConsoleLog].swap(logQueue);
	}
	while (logQueue.empty() == false)
	{
		std::pair<LogType, std::string> p = logQueue.front();
		View(p.first, p.second);
		logQueue.pop();
	}
	{
		WRITE_LOCKS(FileLog);
		_logs[FileLog].swap(logQueue);
	}
	while (logQueue.empty() == false)
	{
		std::pair<LogType, std::string> p = logQueue.front();
		Write(p.first, p.second);
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
				std::queue<std::pair<LogType, std::string>> logQueue;
				{
					WRITE_LOCKS(ConsoleLog);
					_directLogs[ConsoleLog].swap(logQueue);
				}
				while (logQueue.empty() == false)
				{
					const auto& log = logQueue.front();
					View(log.first, log.second);
					logQueue.pop();
				}
				{
					WRITE_LOCKS(FileLog);
					_directLogs[FileLog].swap(logQueue);
				}
				while (logQueue.empty() == false)
				{
					const auto& log = logQueue.front();
					Write(log.first, log.second);
					logQueue.pop();
				}
				{
					WRITE_LOCKS(ConsoleLog);
					_logs[ConsoleLog].swap(logQueue);
				}
				while (logQueue.empty() == false)
				{
					const auto& log = logQueue.front();
					View(log.first, log.second);
					logQueue.pop();
				}
				{
					WRITE_LOCKS(FileLog);
					_logs[FileLog].swap(logQueue);
				}
				while (logQueue.empty() == false)
				{
					const auto& log = logQueue.front();
					Write(log.first, log.second);
					logQueue.pop();
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		});
}

void LogManager::Write(const LogType& type, const std::string& log)
{
	if (_logger == nullptr)
		return;

	switch (type)
	{
	case LogType::Info:
		_logger->info(log);
		break;
	case LogType::Warning:
		_logger->warn(log);
		break;
	case LogType::Error:
		_logger->error(log);
		_logger->flush();
		break;
	default:
		break;
	}
}

void LogManager::View(const LogType& type, const std::string& log)
{
	switch (type)
	{
	case LogType::Info:
		spdlog::info(log);
		break;
	case LogType::Warning:
		spdlog::warn(log);
		break;
	case LogType::Error:
		spdlog::error(log);
		_logger->flush();
		break;
	default:
		break;
	}
}
