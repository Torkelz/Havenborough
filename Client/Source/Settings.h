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
		std::string command;
		Axis axis;
		bool posDir;
	};
	struct HUDSettings
	{
		Vector3 position;
		float scale;
		Vector4 color;
	};
private:
	std::map<std::string, unsigned short> m_KeyMap;
	std::vector<MouseStruct> m_MouseMap;
	std::map<std::string, MouseButton> m_MouseButtonMap;

	std::map<std::string, bool> m_SettingsEnabled;
	std::map<std::string, float> m_SettingsValue;
	std::map<std::string, HUDSettings> m_HUDSettings;
	Vector2 m_Resolution;

	std::string m_ServerURL;
	unsigned short int m_ServerPort;
	std::string m_LevelName;
	std::string m_Username;
	std::string m_CharacterName;
	std::string m_CharacterStyle;


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
	 * Get float value for settings.
	 *
	 * @param p_SettingName, the name of the setting to receive float value of.
	 * @return float value depending on setting read from file.
	 */
	const float getSettingValue(std::string p_SettingName) const;
	/*
	 * Get the window size which is read from file.
	 * If no resolution was read from file the default value is (1280,720)
	 *
	 * @return the window size as a Vector2.
	 */
	const Vector2 getResolution() const;
	/**
	 * Gets the name of the level to play.
	 *
	 * @return the level name
	 */
	const std::string& getLevelName() const;
	/**
	 * Gets the username to display to other players.
	 *
	 * @return the players name
	 */
	const std::string& getUsername() const;
	/**
	 * Gets the username to display to other players.
	 *
	 * @return the players name
	 */
	const std::string& getCharacterName() const;
	/**
	 * Gets the username to display to other players.
	 *
	 * @return the players name
	 */
	const std::string& geCharacterStyle() const;
	/**
	 * Gets the URL address of the server to connect to.
	 *
	 * @return a string representation of a server address
	 */
	const std::string& getServerURL() const;
	/**
	 * Gets the port number to connect to on the server.
	 *
	 * @return the port number, [0, 65535]
	 */
	unsigned short int getServerPort() const;
	/**
	 * Gets the port number to connect to on the server.
	 *
	 * @return the port number, [0, 65535]
	 */
	std::map<std::string, HUDSettings> getHUDSettings() const;

private:
	void loadControls(tinyxml2::XMLElement *p_Element);
	void loadSettings(tinyxml2::XMLElement *p_Element);
	void loadGame(const tinyxml2::XMLElement *p_Element);
	void loadServer(const tinyxml2::XMLElement *p_Element);
	void loadCharacter(const tinyxml2::XMLElement *p_Element);
	void loadHUD(tinyxml2::XMLElement *p_Element);
};
