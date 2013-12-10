#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <ostream>
#include <vector>

class Logger
{
public:
	enum class Level : uint32_t
	{
		TRACE = 0,
		DEBUG = 1,
		INFO = 2,
		WARNING = 3,
		ERROR_L = 4,
		FATAL = 5,

		ALL = TRACE,
	};

private:
	static std::mutex m_Mutex;
	static std::unique_ptr<Logger> m_Instance;

	struct Output
	{
		Level m_Level;
		std::ostream& m_Destination;
	};

	std::vector<Output> m_Outputs;

public:
	static void log(Level p_Level, const char* p_Message);
	static void addOutput(Level p_Level, std::ostream& p_Out);

private:
	static Logger* getInstance();
};
