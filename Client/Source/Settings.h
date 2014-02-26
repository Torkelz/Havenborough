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
private:
	std::map<std::string, unsigned short> m_KeyMap;
	std::vector<MouseStruct> m_MouseMap;
	std::map<std::string, MouseButton> m_MouseButtonMap;

	std::map<std::string, bool> m_SettingsEnabled;
	Vector2 m_Resolution;
	int m_ShadowMapResolution;
public:
	Settings(void);
	~Settings(void);

	/*
	 * Initialize and load the settings file.
	 * Throws an Client exception if any error occurs.
	 *
	 * @param p_FilePath, the path and filename to the .xml settings file.
	 */
	void initialize(std::string p_FilePath);

	/*
	 * Get key mapping.
	 *
	 * @return the keymapping read from options file.
	 */
	const std::map<std::string, unsigned short> &getKeyMap() const;
	/*
	 * Get the mouse mapping.
	 *
	 * @return the mouse mapping read from options file.
	 */
	const std::vector<MouseStruct> &getMouseMap() const;
	/*
	 * Get the mouse button mapping.
	 *
	 * @return the mouse button mapping read from options file.
	 */
	const std::map<std::string, MouseButton> &getMouseButtonMap() const;
	/*
	 * Get true or false for settings.
	 *
	 * @param p_SettingName, the name of the setting to receive if it is true or false.
	 * @return true or false depending on setting read from file.
	 */
	const bool getIsSettingEnabled(std::string p_SettingName) const;
	/*
	 * Get the window size which is read from file.
	 * If no resolution was read from file the default value is (1280,720)
	 *
	 * @return the window size as a Vector2.
	 */
	const Vector2 getResolution() const;
	/*
	 * Get the shadow map resolution from settings.
	 * If no resolution was read from file the default value is (512)
	 *
	 * @return the map size as an integer.
	 */
	const int getShadowMapResolution() const;
private:
	void loadControls(tinyxml2::XMLElement *p_Element);
	void loadSettings(tinyxml2::XMLElement *p_Element);
};

