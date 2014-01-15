#include "SoundLogger.h"

ISound::clientLogCallback_t SoundLogger::m_LogFunc = nullptr;

void SoundLogger::log(Level p_Level, const std::string& p_Message)
{
	if (m_LogFunc)
	{
		m_LogFunc((uint32_t)p_Level, p_Message.c_str());
	}
}

void SoundLogger::setLogFunction(ISound::clientLogCallback_t p_LogCallback)
{
	m_LogFunc = p_LogCallback;
}
