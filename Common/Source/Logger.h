#pragma once
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <vector>

/**
 * Log managing singleton. Prints logs to any number of
 * output streams depending on priority levels.
 */
class Logger
{
public:
	/**
	 * Log priority level. Higher is more important.
	 */
	enum class Level : uint32_t
	{
		TRACE = 0,
		DEBUG_L = 1,
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
	/**
	 * Add a log message.
	 *
	 * @param p_Level the priority level of the log message.
	 * @param p_Message the message to log.
	 */
	static void log(Level p_Level, const std::string& p_Message);
	
	/**
	 * Add a raw log message. Prefer {#log(Level, const std::string&)} for normal usage.
	 *
	 * @param p_Level the priority level of the log message.
	 * @param p_Message the message to log.
	 */
	static void logRaw(uint32_t p_Level, const char* p_Message);

	/**
	 * Add an output stream to print log messages to.
	 *
	 * @param p_Level the level of log messages to print to the stream.
	 *			Messages of equal or higher priorities will be sent.
	 * @param p_Out an output stream to receive formatted log messages.
	 */
	static void addOutput(Level p_Level, std::ostream& p_Out);

	/**
	 * Reset the logger, to clear away any added output streams.
	 */
	static void reset();

private:
	static Logger* getInstance();
};
