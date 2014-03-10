#include "Settings.h"
#include "ClientExceptions.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> //For VK_* defines

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
			const char* value = element->Attribute("Key");
			if(!value)
				throw ClientException("Settings tried to load the attribute \"key\" from element: " + elementName + ".", __LINE__, __FILE__);

			std::string sValue(value);
			unsigned short keyValue;

			if (sValue.length() == 1 && sValue[0] >= 'A' && sValue[0] <= 'Z')
			{
				keyValue = sValue[0];
			}
			else
			{
				if(sValue == "Escape")
					keyValue = VK_ESCAPE;
				else if(sValue == "Space")
					keyValue = VK_SPACE;
				else if(sValue == "Return")
					keyValue = VK_RETURN;
				else if(sValue == "LShift")
					keyValue = VK_LSHIFT;
				else if(sValue == "RShift")
					keyValue = VK_RSHIFT;
				else if(sValue == "LCtrl")
					keyValue = VK_LCONTROL;
				else if(sValue == "RCtrl")
					keyValue = VK_RCONTROL;
				else
				{
					try
					{
						int val = std::stoi(sValue);
#undef max
						if (val < 0 || val > std::numeric_limits<unsigned short>::max())
						{
							throw std::out_of_range("Value is not a short");
						}
						keyValue = (unsigned short)val;
					}
					catch (std::invalid_argument* e)
					{
						throw ClientException("Settings tried to map \""+sValue+"\"." + std::string(e->what()), __LINE__, __FILE__);
					}
					catch (std::out_of_range* e)
					{
						throw ClientException("Settings tried to map \""+sValue+"\". The value is out of range. " + std::string(e->what()), __LINE__, __FILE__);
					}
				}
			}

			m_KeyMap.insert(make_pair(std::string(commandValue), keyValue));
		}
		else if(elementName == "MouseMap")
		{
			const char* pValue = element->Attribute("Position");
			const char* mValue = element->Attribute("Movement");
			if (!mValue || !pValue)
				throw ClientException("Settings tried to load the attribute \"movement\" or \"position\" from element: " + elementName + ".", __LINE__, __FILE__);

			MouseStruct m;
			m.movement = std::string(commandValue);
			m.position = pValue;
			std::string keyValue(mValue);

			if(keyValue == "Vertical")
				m.axis = Axis::VERTICAL;
			else if(keyValue == "Horizontal")
				m.axis = Axis::HORIZONTAL;

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

		tinyxml2::XMLError res;
		HUDSettings hudSett;
		res = element->QueryFloatAttribute("x", &hudSett.position.x);
		if(res == tinyxml2::XML_SUCCESS)
			res = element->QueryFloatAttribute("y", &hudSett.position.y);
		if(res == tinyxml2::XML_SUCCESS)
			res = element->QueryFloatAttribute("z", &hudSett.position.z);

		if(res != tinyxml2::XML_SUCCESS)
				throw ClientException("Settings tried to load the position from element: " + elementName + ".", __LINE__, __FILE__);

		res = element->QueryFloatAttribute("scale", &hudSett.scale);
		if(res != tinyxml2::XML_SUCCESS)
				throw ClientException("Settings tried to load the scale from element: " + elementName + ".", __LINE__, __FILE__);
		
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
