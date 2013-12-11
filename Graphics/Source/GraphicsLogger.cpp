#include "GraphicsLogger.h"

IGraphics::clientLogCallback_t GraphicsLogger::m_LogFunc = nullptr;

void GraphicsLogger::log(Level p_Level, const char* p_Message)
{
	if (m_LogFunc)
	{
		m_LogFunc((uint32_t)p_Level, p_Message);
	}
}

void GraphicsLogger::setLogFunction(IGraphics::clientLogCallback_t p_LogCallback)
{
	m_LogFunc = p_LogCallback;
}
