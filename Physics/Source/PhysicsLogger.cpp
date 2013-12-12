#include "PhysicsLogger.h"

IPhysics::clientLogCallback_t PhysicsLogger::m_LogFunc = nullptr;

void PhysicsLogger::log(Level p_Level, const std::string& p_Message)
{
	if (m_LogFunc)
	{
		m_LogFunc((uint32_t)p_Level, p_Message.c_str());
	}
}

void PhysicsLogger::setLogFunction(IPhysics::clientLogCallback_t p_LogCallback)
{
	m_LogFunc = p_LogCallback;
}
