#include "Settings.h"
#include "ClientExceptions.h"
#include <XMLHelper.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> //For VK_* defines

#include <tuple>
#include <boost/algorithm/string.hpp>

Settings::Settings(void)
{
	m_Resolution = Vector2(1080,720);
	m_SettingsValue["ShadowMapResolution"] = 512.f;
	m_LevelName = "serverLevel";
	m_Username = "Player";
	m_ServerURL = "localhost";
	m_ServerPort = 31415;
	m_CharacterName = "Dzala";
	m_CharacterStyle = "Black";
}


Settings::~Settings(void)
{
}

void Settings::initialize(std::string p_FilePath)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError res = doc.LoadFile(p_FilePath.c_str());
	if(res != tinyxml2::XML_NO_ERROR)
		throw ClientException("Settings file was not found.", __LINE__, __FILE__);

	tinyxml2::XMLElement *root = doc.FirstChildElement();
	if(root == nullptr)
		throw ClientException("Failed to load settings file. Root element not found.", __LINE__, __FILE__);

	tinyxml2::XMLElement *element = nullptr;
	for(element = root->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
	{
		std::string elementName = element->Value();
		if(elementName == "Settings")
		{
			loadSettings(element);
		}
		else if(elementName == "Controls")
		{
			loadControls(element);
		}
		else if (elementName == "Game")
		{
			loadGame(element);
		}
		else if (elementName == "Character")
		{
			loadCharacter(element);
		}
		else if (elementName == "Server")
		{
			loadServer(element);
		}
		else if(elementName == "HUD")
		{
			loadHUD(element);
		}
	}
}

void Settings::loadControls(tinyxml2::XMLElement *p_Element)
{
	tinyxml2::XMLElement *element = nullptr;
	for(element = p_Element->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
	{
		std::string elementName = element->Value();
		const char* commandValue = element->Attribute("Command");
		if(!commandValue)
			throw ClientException("Settings tried to load the attribute \"command\" from element: " + elementName + ".", __LINE__, __FILE__);

		if(elementName == "KeyMap")
		{
			int value = -1;
			element->QueryAttribute("Key", &value);
			if(value < 0)
				throw ClientException("Settings tried to load the attribute \"Key\" from element: " + elementName + ".", __LINE__, __FILE__);

			m_KeyMap.insert(make_pair(std::string(commandValue), value));
		}
		else if(elementName == "MouseMap")
		{
			const char* direction = element->Attribute("Direction");
			if (!direction)
				throw ClientException("Settings tried to load the attribute \"Direction\" from element: " + elementName + ".", __LINE__, __FILE__);

			std::string sDir(direction);

			static const std::tuple<std::string, Axis, bool> mouseDirs[] =
			{
				std::make_tuple("NegX", Axis::HORIZONTAL, false),
				std::make_tuple("PosX", Axis::HORIZONTAL, true),
				std::make_tuple("NegY", Axis::VERTICAL, false),
				std::make_tuple("PosY", Axis::VERTICAL, true),
			};

			MouseStruct m;
			bool foundDir = false;

			for (const auto& dir : mouseDirs)
			{
				if (boost::iequals(std::get<0>(dir), sDir))
				{
					m.axis = std::get<1>(dir);
					m.posDir = std::get<2>(dir);
					foundDir = true;
					break;
				}
			}

			if (!foundDir)
			{
				throw ClientException(std::string("Invalid setting for \"") + commandValue + std::string("\" in MouseMap"), __LINE__, __FILE__);
			}

			m.command = commandValue;

			m_MouseMap.push_back(m);
		}
		else if(elementName == "MouseButtonMap")
		{
			const char* bValue = element->Attribute("Button");
			if (!bValue)
				throw ClientException("Settings tried to load the attribute \"button\" from element: " + elementName + ".", __LINE__, __FILE__);

			std::string keyValue(bValue);
			MouseButton value;
			if(keyValue == "Left")
				value = MouseButton::LEFT;
			else if(keyValue == "Right")
				value = MouseButton::RIGHT;
			else if(keyValue == "Middle")
				value = MouseButton::MIDDLE;
			else if(keyValue == "Extra_1")
				value = MouseButton::EXTRA_1;
			else if(keyValue == "Extra_2")
				value = MouseButton::EXTRA_2;

			m_MouseButtonMap.insert(make_pair(std::string(commandValue), value));
		}
	}
}

void Settings::loadSettings(tinyxml2::XMLElement *p_Element)
{
	tinyxml2::XMLElement *element = nullptr;
	for(element = p_Element->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
	{
		std::string elementName = element->Value();
		if(elementName == "Resolution")
		{
			tinyxml2::XMLError res;
			res = element->QueryFloatAttribute("Width", &m_Resolution.x);
			if(res == tinyxml2::XML_SUCCESS)
				res = element->QueryFloatAttribute("Height", &m_Resolution.y);

			if(res != tinyxml2::XML_SUCCESS)
				throw ClientException("Settings tried to load the attribute \"height\" or \"width\" from element: " + elementName + ".", __LINE__, __FILE__);
		}
		else
		{
			bool enabled = false;
			float value = 0.f;
			tinyxml2::XMLError res;
			res = element->QueryBoolAttribute("Enabled", &enabled);
			if(res == tinyxml2::XML_NO_ATTRIBUTE)
			{
				res = element->QueryFloatAttribute("Value", &value);
				m_SettingsValue[elementName] = value;
			}
			else
			{
				m_SettingsEnabled[elementName] = enabled;
			}

			if(res != tinyxml2::XML_SUCCESS)
				throw ClientException("Settings tried to load the attribute \"enabled\" from element: " + elementName + ".", __LINE__, __FILE__);
		}
	}
}

void Settings::loadGame(const tinyxml2::XMLElement *p_Element)
{
	const char* level = p_Element->Attribute("Level");
	if (level)
	{
		m_LevelName = level;
	}

	const char* username = p_Element->Attribute("Username");
	if (username)
	{
		m_Username = username;
	}
}

void Settings::loadCharacter(const tinyxml2::XMLElement *p_Element)
{
	const char* name = p_Element->Attribute("Name");
	if (name)
	{
		m_CharacterName = name;
	}

	const char* style = p_Element->Attribute("Style");
	if (style)
	{
		m_CharacterStyle = style;
	}
}

void Settings::loadServer(const tinyxml2::XMLElement *p_Element)
{
	const char* address = p_Element->Attribute("Hostname");
	if (address)
	{
		m_ServerURL = address;
	}

	unsigned int tPort = m_ServerPort;
	p_Element->QueryAttribute("Port", &tPort);
#undef max
	if (tPort <= std::numeric_limits<uint16_t>::max())
	{
		m_ServerPort = tPort;
	}
}

void Settings::loadHUD(tinyxml2::XMLElement *p_Element)
{
	tinyxml2::XMLElement *element = nullptr;
	for(element = p_Element->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
	{
		std::string elementName = element->Value();
		if(m_HUDSettings.count(elementName) > 0)
				throw ClientException("Settings tried to load an already loaded element: " + elementName + ".", __LINE__, __FILE__);

		HUDSettings hudSett;
		hudSett.color = Vector4(1.f, 1.f, 1.f, 1.f);
		hudSett.position = Vector3(0.f, 0.f, 0.f);
		hudSett.scale = 1.f;

		queryColor(element->FirstChildElement("Color"), hudSett.color);
		queryVector(element->FirstChildElement("Position"), hudSett.position);
		element->QueryFloatAttribute("scale", &hudSett.scale);
		
		m_HUDSettings.insert(std::pair<std::string, HUDSettings>(elementName, hudSett));
	}
}

const std::map<std::string, unsigned short> &Settings::getKeyMap() const
{
	return m_KeyMap;
}

const std::vector<Settings::MouseStruct> &Settings::getMouseMap() const
{
	return m_MouseMap;
}

const std::map<std::string, MouseButton> &Settings::getMouseButtonMap() const
{
	return m_MouseButtonMap;
}

const bool Settings::getIsSettingEnabled(std::string p_SettingName) const
{
	if (m_SettingsEnabled.count(p_SettingName) > 0)
	{
		return m_SettingsEnabled.at(p_SettingName);
	} 
	else
	{
		throw ClientException("The setting with name: " + p_SettingName + ", was not found.", __LINE__, __FILE__);
	}
}

const float Settings::getSettingValue(std::string p_SettingName) const
{
	if (m_SettingsValue.count(p_SettingName) > 0)
	{
		return m_SettingsValue.at(p_SettingName);
	} 
	else
	{
		throw ClientException("The setting with name: " + p_SettingName + ", was not found.", __LINE__, __FILE__);
	}
}

const Vector2 Settings::getResolution() const
{
	return m_Resolution;
}

const std::string& Settings::getLevelName() const
{
	return m_LevelName;
}

const std::string& Settings::getUsername() const
{
	return m_Username;
}

const std::string& Settings::getCharacterName() const
{
	return m_CharacterName;
}

const std::string& Settings::geCharacterStyle() const
{
	return m_CharacterStyle;
}

const std::string& Settings::getServerURL() const
{
	return m_ServerURL;
}

unsigned short int Settings::getServerPort() const
{
	return m_ServerPort;
}

std::map<std::string, Settings::HUDSettings> Settings::getHUDSettings() const
{
	return m_HUDSettings;
}
