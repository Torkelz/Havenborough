#include "Logger.h"

#include <iomanip>
#include <string>

std::mutex Logger::m_Mutex;
std::unique_ptr<Logger> Logger::m_Instance;

void Logger::log(Level p_Level, const std::string& p_Message)
{
	logRaw((uint32_t)p_Level, p_Message.c_str());
}

void Logger::logRaw(uint32_t p_Level, const char* p_Message)
{
	Logger* instance = getInstance();

	std::unique_lock<std::mutex> lock(m_Mutex);

	time_t currentTime = time(nullptr);
#pragma warning (suppress : 4996)
	tm* currentLocalTime = localtime(&currentTime);

	static const std::string levelNames[] =
	{
		"TRACE",
		"DEBUG",
		"INFO",
		"WARNING",
		"ERROR",
		"FATAL",
	};

	for (Output& out : instance->m_Outputs)
	{
		if (p_Level >= (uint32_t)out.m_Level)
		{
			out.m_Destination << std::put_time(currentLocalTime, "[%Y-%m-%d %H:%M:%S]") << " " << levelNames[(uint32_t)p_Level] << ": " << p_Message << std::endl << std::flush;
		}
	}
}

void Logger::addOutput(Level p_Level, std::ostream& p_Out)
{
	Logger* instance = getInstance();

	std::unique_lock<std::mutex> lock(m_Mutex);

	Output out = { p_Level, p_Out };
	instance->m_Outputs.push_back(out);
}

void Logger::reset()
{
	std::unique_lock<std::mutex> lock(m_Mutex);

	m_Instance.reset();
}

Logger* Logger::getInstance()
{
	if (!m_Instance)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);

		if (!m_Instance)
		{
			m_Instance.reset(new Logger());
		}
	}

	return m_Instance.get();
}
