#include "NetworkLogger.h"

INetwork::clientLogCallback_t NetworkLogger::m_LogFunc = nullptr;

void NetworkLogger::log(Level p_Level, const std::string& p_Message)
{
	if (m_LogFunc)
	{
		m_LogFunc((uint32_t)p_Level, p_Message.c_str());
	}
}

void NetworkLogger::setLogFunction(INetwork::clientLogCallback_t p_LogCallback)
{
	m_LogFunc = p_LogCallback;
}
