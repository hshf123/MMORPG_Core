#include "pch.h"
#include "LogManager.h"
#include "ThreadManager.h"

LogManager::LogManager()
{

}

LogManager::~LogManager()
{
	{
		WRITE_LOCKS(ConsoleLog);
		auto q = _logs[ConsoleLog];
		while (_logs[ConsoleLog].empty() == false)
		{
			std::pair<LogType, std::string> p = q.front();
			View(p.first, p.second);
			q.pop();
		}
	}
	{
		WRITE_LOCKS(FileLog);
		auto q = _logs[FileLog];
		while (_logs[FileLog].empty() == false)
		{
			std::pair<LogType, std::string> p = q.front();
			Write(p.first, p.second);
			q.pop();
		}
	}
}

void LogManager::Initialize(const std::string& loggerName /*= "basic_logger"*/, const std::string& filepath /*= "logs/basic-log.txt"*/)
{
	try
	{
		_logger = spdlog::basic_logger_mt(loggerName, filepath);
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
				if (_logs[ConsoleLog].empty() == false)
				{
					WRITE_LOCKS(ConsoleLog);
					const auto& log = _logs[ConsoleLog].front();
					View(log.first, log.second);
					_logs[ConsoleLog].pop();
				}
				if (_logs[FileLog].empty() == false)
				{
					WRITE_LOCKS(FileLog);
					const auto& log = _logs[FileLog].front();
					Write(log.first, log.second);
					_logs[FileLog].pop();
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
		break;
	default:
		break;
	}
}
