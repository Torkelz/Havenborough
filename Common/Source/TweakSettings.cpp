#include "TweakSettings.h"

#include "CommonExceptions.h"
#include "Logger.h"

#include <boost/algorithm/string.hpp>
#include <tinyxml2/tinyxml2.h>

std::unique_ptr<TweakSettings> TweakSettings::m_MasterInstance;
TweakSettings* TweakSettings::m_Instance = nullptr;

TweakSettings* TweakSettings::getInstance()
{
	if (!m_Instance)
	{
		throw CommonException("TweakSettings has not been initialized", __LINE__, __FILE__);
	}

	return m_Instance;
}

void TweakSettings::initializeMaster()
{
	m_MasterInstance.reset(new TweakSettings);
	m_Instance = m_MasterInstance.get();
}

void TweakSettings::initializeSlave(TweakSettings* p_Master)
{
	m_Instance = p_Master;
}

void TweakSettings::shutdown()
{
	m_Instance = nullptr;
	m_MasterInstance.reset();
}

void TweakSettings::saveSettings(std::ostream& p_Out) const
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("TweakSettings");

	for (const auto& setting : m_Settings)
	{
		printer.OpenElement("Setting");
		printer.PushAttribute("Name", setting.first.c_str());
		printer.PushAttribute("Value", setting.second.value.c_str());
		printer.CloseElement();
	}

	printer.CloseElement();

	p_Out << printer.CStr();
}

void TweakSettings::loadSettings(std::istream& p_In)
{
	std::streamoff streamsize = getStreamsize(p_In);
	if (streamsize == 0)
	{
		throw InvalidArgument("Invalid stream", __LINE__, __FILE__);
	}

	std::vector<char> buffer((size_t)streamsize);
	p_In.read(buffer.data(), buffer.size());

	tinyxml2::XMLDocument doc;
	doc.Parse(buffer.data(), buffer.size());

	const tinyxml2::XMLElement* root = doc.FirstChildElement("TweakSettings");
	if (!root)
	{
		throw InvalidArgument("XML missing root element 'TweakSettings'", __LINE__, __FILE__);
	}

	for (const tinyxml2::XMLElement* setting = root->FirstChildElement("Setting");
		setting;
		setting = setting->NextSiblingElement("Setting"))
	{
		const char* name = setting->Attribute("Name");
		const char* value = setting->Attribute("Value");

		if (name && value)
		{
			setSetting(name, std::string(value));
		}
	}
}

std::vector<std::string> TweakSettings::getSettings() const
{
	std::vector<std::string> result;
	for (const auto& setting : m_Settings)
	{
		result.push_back(setting.first + " = " + setting.second.value);
	}
	return result;
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

void TweakSettings::querySetting(const std::string& p_Setting, bool& p_OutVal) const
{
	auto setting = m_Settings.find(p_Setting);
	if (setting != m_Settings.end())
	{
		const std::string& value = setting->second.value;
		if (boost::iequals(value, "true"))
		{
			p_OutVal = true;
		}
		else if (boost::iequals(value, "false"))
		{
			p_OutVal = false;
		}
	}
}

void TweakSettings::setSetting(const std::string& p_Setting, const std::string& p_Val)
{
	m_Settings[p_Setting].value = p_Val;

	auto& listener = m_Settings[p_Setting].listener;
	if (listener)
	{
		try
		{
			listener(m_Settings[p_Setting].value);
		}
		catch (const std::exception& p_Ex)
		{
			Logger::log(Logger::Level::WARNING, std::string("Failed to call tweak listener: ") + p_Ex.what());
		}
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

void TweakSettings::setSetting(const std::string& p_Setting, bool p_Val)
{
	setSetting(p_Setting, p_Val ? std::string("true") : std::string("false"));
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

void TweakSettings::setListener(const std::string& p_Setting, std::function<void(bool)> p_Listener)
{
	m_Settings[p_Setting].listener =
		[p_Listener] (const std::string& p_Value)
		{
			if (boost::iequals(p_Value, "true"))
			{
				p_Listener(true);
			}
			else if (boost::iequals(p_Value, "false"))
			{
				p_Listener(false);
			}
		};
}

std::streamoff TweakSettings::getStreamsize(std::istream& p_Stream)
{
	p_Stream.seekg(0, std::istream::end);
	std::streampos streamsize = p_Stream.tellg();
	p_Stream.seekg(0, std::istream::beg);

	return std::max(std::streampos(0), streamsize);
}
