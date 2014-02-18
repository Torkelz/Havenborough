#pragma once
#include "Input/InputTranslatorEnums.h"
#include "Utilities/XMFloatUtil.h"

#include <tinyxml2/tinyxml2.h>
#include <string>
#include <vector>
#include <map>

class Settings
{
public:
	struct MouseStruct
	{
		std::string movement;
		std::string position;
		Axis axis;
	};

	typedef std::vector<std::pair<std::string, unsigned short>> vectorpairKeyMap;
	typedef std::vector<std::pair<std::string, MouseButton>> vectorpairMouseButtonMap;
private:
	vectorpairKeyMap m_KeyMap;
	std::vector<MouseStruct> m_MouseMap;
	vectorpairMouseButtonMap m_MouseButtonMap;

	std::map<std::string, bool> m_SettingsEnabled;
	Vector2 m_Resolution;
public:
	Settings(void);
	~Settings(void);

	/*
	 * 
	 */
	void initialize(std::string p_FilePath);

	const vectorpairKeyMap &getKeyMap() const;
	const std::vector<MouseStruct> &getMouseMap() const;
	const vectorpairMouseButtonMap &getMouseButtonMap() const;
	const bool getIsSettingEnabled(std::string p_SettingName) const;
	const Vector2 getResolution() const;
private:
	void loadControls(tinyxml2::XMLElement *p_Element);
	void loadSettings(tinyxml2::XMLElement *p_Element);
};

