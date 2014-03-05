#pragma once

#include <map>
#include <memory>
#include <string>

class DebugInfo
{
public:
	typedef std::map<std::string, std::string> InfoList;
	typedef std::shared_ptr<DebugInfo> ptr;

private:
	InfoList m_InfoList;

public:
	void updateDebugInfo(const std::string& p_Key, const std::string& p_Value);
	const InfoList& getInfoList() const;
};
