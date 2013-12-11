#include "PhysicsLogger.h"

IPhysics::clientLogCallback_t PhysicsLogger::m_LogFunc = nullptr;

void PhysicsLogger::log(Level p_Level, const char* p_Message)
{
	if (m_LogFunc)
	{
		m_LogFunc((uint32_t)p_Level, p_Message);
	}
}

void PhysicsLogger::setLogFunction(IPhysics::clientLogCallback_t p_LogCallback)
{
	m_LogFunc = p_LogCallback;
}
