#include "TweakSettings.h"

std::unique_ptr<TweakSettings> TweakSettings::m_Instance;

TweakSettings* TweakSettings::getInstance()
{
	if (!m_Instance)
	{
		m_Instance.reset(new TweakSettings);
	}

	return m_Instance.get();
}

void TweakSettings::shutdown()
{
	m_Instance.reset();
}

void TweakSettings::querySetting(const std::string& p_Setting, std::string& p_OutVal) const
{
	auto setting = m_Settings.find(p_Setting);
	if (setting != m_Settings.end())
	{
		p_OutVal = setting->second.value;
	}
}

void TweakSettings::querySetting(const std::string& p_Setting, float& p_OutVal) const
{
	auto setting = m_Settings.find(p_Setting);
	if (setting != m_Settings.end())
	{
		p_OutVal = std::stof(setting->second.value);
	}
}

void TweakSettings::querySetting(const std::string& p_Setting, int& p_OutVal) const
{
	auto setting = m_Settings.find(p_Setting);
	if (setting != m_Settings.end())
	{
		p_OutVal = std::stoi(setting->second.value);
	}
}

void TweakSettings::setSetting(const std::string& p_Setting, const std::string& p_Val)
{
	m_Settings[p_Setting].value = p_Val;

	auto& listener = m_Settings[p_Setting].listener;
	if (listener)
	{
		listener(m_Settings[p_Setting].value);
	}
}

void TweakSettings::setSetting(const std::string& p_Setting, float p_Val)
{
	setSetting(p_Setting, std::to_string(p_Val));
}

void TweakSettings::setSetting(const std::string& p_Setting, int p_Val)
{
	setSetting(p_Setting, std::to_string(p_Val));
}

void TweakSettings::setListener(const std::string& p_Setting, std::function<void(const std::string&)> p_Listener)
{
	m_Settings[p_Setting].listener =
		[p_Listener] (const std::string& p_Value)
		{
			p_Listener(p_Value);
		};
}

void TweakSettings::setListener(const std::string& p_Setting, std::function<void(float)> p_Listener)
{
	m_Settings[p_Setting].listener =
		[p_Listener] (const std::string& p_Value)
		{
			p_Listener(std::stof(p_Value));
		};
}

void TweakSettings::setListener(const std::string& p_Setting, std::function<void(int)> p_Listener)
{
	m_Settings[p_Setting].listener =
		[p_Listener] (const std::string& p_Value)
		{
			p_Listener(std::stoi(p_Value));
		};
}
