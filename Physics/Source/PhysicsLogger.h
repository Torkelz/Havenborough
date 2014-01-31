/**
 * Licence.
 */

#pragma once
#include <IPhysics.h>

#include <string>

/**
 * Wrapper for a log function.
 */
class PhysicsLogger
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
	};

private:
	static IPhysics::clientLogCallback_t m_LogFunc;

public:
	/**
	 * Add a log message.
	 *
	 * @param p_Level the priority level of the log message.
	 * @param p_Message the message to log.
	 */
	static void log(Level p_Level, const std::string& p_Message);

	/**
	 * Set the log function to use for logging.
	 *
	 * @param p_LogCallback a function that will be called for each log recieved.
	 *			Set to null to disable logging.
	 */
	static void setLogFunction(IPhysics::clientLogCallback_t p_LogCallback);
};
