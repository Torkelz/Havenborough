#pragma once

#include <IPhysics.h>

#include <cstdint>

class PhysicsLogger
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
	};

private:
	static IPhysics::clientLogCallback_t m_LogFunc;

public:
	static void log(Level p_Level, const char* p_Message);

	static void setLogFunction(IPhysics::clientLogCallback_t p_LogCallback);
};
