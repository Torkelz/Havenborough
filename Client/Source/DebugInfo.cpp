#include "DebugInfo.h"

void DebugInfo::updateDebugInfo(const std::string& p_Key, const std::string& p_Value)
{
	m_InfoList[p_Key] = p_Value;
}

const DebugInfo::InfoList& DebugInfo::getInfoList() const
{
	return m_InfoList;
}
