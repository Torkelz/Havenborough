/**
 *
 */
#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

/**
 * System for managing the tweaking of settings during runtime.
 *
 * To change a setting, simply call {@link #setSetting} with the new value.
 * To allow a setting to be tweakable, use {@link #setListener} to listen for value changes.
 */
class TweakSettings
{
private:
	typedef std::function<void(const std::string&)> CastingListener;

	struct Setting
	{
		std::string value;
		CastingListener listener;
	};

	static std::unique_ptr<TweakSettings> m_MasterInstance;
	static TweakSettings* m_Instance;

	std::map<const std::string, Setting> m_Settings;

public:
	/**
	 * Gets the instance of the system.
	 *
	 * @return a object ready to provide tweaking
	 * @throws CommonException if the system has not yet been initialized.
	 */
	static TweakSettings* getInstance();

	/**
	 * Initializes the master system, allowing the system to be used from this binary.
	 */
	static void initializeMaster();
	/**
	 * Initializes the slave, which allows the system to be used from a
	 * binary different from the one where the master exists.
	 *
	 * @param p_Master the master this slave should use for its funtions.
	 */
	static void initializeSlave(TweakSettings* p_Master);
	/**
	 * Cleans up after the system. Only has any real use when called from the master binary.
	 */
	static void shutdown();

	/**
	 * Outputs a formatted xml text to the stream, describing all existing settings.
	 *
	 * @param p_Out a stream that can accept text output
	 */
	void saveSettings(std::ostream& p_Out) const;
	/**
	 * Loads settings from a formatted xml stream.
	 *
	 * If a setting is not read from the stream, that setting will keep its value,
	 * while any value read from the stream will be overwritte.
	 *
	 * @param p_In a stream where text can be read.
	 *		The stream will be left at the end of the stream.
	 */
	void loadSettings(std::istream& p_In);

	/**
	 * Gets a list of string representations of the currently set settings.
	 *
	 * @return a list of formatted settings
	 */
	std::vector<std::string> getSettings() const;

	/**
	 * Query for a particular setting.
	 *
	 * @param p_Setting the setting to query
	 * @param p_OutVal the value to set to the value of the setting.
	 *			Does not change if the setting is not set.
	 */
	void querySetting(const std::string& p_Setting, std::string& p_OutVal) const;
	/**
	 * Query for a particular setting.
	 *
	 * @param p_Setting the setting to query
	 * @param p_OutVal the value to set to the value of the setting.
	 *			Does not change if the setting is not set.
	 */
	void querySetting(const std::string& p_Setting, float& p_OutVal) const;
	/**
	 * Query for a particular setting.
	 *
	 * @param p_Setting the setting to query
	 * @param p_OutVal the value to set to the value of the setting.
	 *			Does not change if the setting is not set.
	 */
	void querySetting(const std::string& p_Setting, int& p_OutVal) const;
	/**
	 * Query for a particular setting.
	 *
	 * @param p_Setting the setting to query
	 * @param p_OutVal the value to set to the value of the setting.
	 *			Does not change if the setting is not set.
	 */
	void querySetting(const std::string& p_Setting, bool& p_OutVal) const;

	/**
	 * Sets the value of a setting to the specified value, overwriting any existing value.
	 *
	 * If the setting does not already exist, it will be created.
	 *
	 * @param p_Setting the setting to change
	 * @param p_Val the value to set for the setting
	 */
	void setSetting(const std::string& p_Setting, const std::string& p_Val);
	/**
	 * Sets the value of a setting to the specified value, overwriting any existing value.
	 *
	 * If the setting does not already exist, it will be created.
	 *
	 * @param p_Setting the setting to change
	 * @param p_Val the value to set for the setting
	 */
	void setSetting(const std::string& p_Setting, float p_Val);
	/**
	 * Sets the value of a setting to the specified value, overwriting any existing value.
	 *
	 * If the setting does not already exist, it will be created.
	 *
	 * @param p_Setting the setting to change
	 * @param p_Val the value to set for the setting
	 */
	void setSetting(const std::string& p_Setting, int p_Val);
	/**
	 * Sets the value of a setting to the specified value, overwriting any existing value.
	 *
	 * If the setting does not already exist, it will be created.
	 *
	 * @param p_Setting the setting to change
	 * @param p_Val the value to set for the setting
	 */
	void setSetting(const std::string& p_Setting, bool p_Val);

	/**
	 * Sets a listener for a specified setting.
	 *
	 * The listener will be called whenever the setting is set.
	 * A empty listener can be used to disable listening for the specific setting.
	 *
	 * @param p_Setting the setting to set a listener for
	 * @param p_Listener the listener that should be called when the setting change
	 */
	void setListener(const std::string& p_Setting, std::function<void(const std::string&)> p_Listener);

	/**
	 * Sets a listener for a specified setting.
	 *
	 * The listener will be called whenever the setting is set.
	 * A empty listener can be used to disable listening for the specific setting.
	 *
	 * @param p_Setting the setting to set a listener for
	 * @param p_Listener the listener that should be called when the setting change
	 */
	void setListener(const std::string& p_Setting, std::function<void(float)> p_Listener);

	/**
	 * Sets a listener for a specified setting.
	 *
	 * The listener will be called whenever the setting is set.
	 * A empty listener can be used to disable listening for the specific setting.
	 *
	 * @param p_Setting the setting to set a listener for
	 * @param p_Listener the listener that should be called when the setting change
	 */
	void setListener(const std::string& p_Setting, std::function<void(int)> p_Listener);

	/**
	 * Sets a listener for a specified setting.
	 *
	 * The listener will be called whenever the setting is set.
	 * A empty listener can be used to disable listening for the specific setting.
	 *
	 * @param p_Setting the setting to set a listener for
	 * @param p_Listener the listener that should be called when the setting change
	 */
	void setListener(const std::string& p_Setting, std::function<void(bool)> p_Listener);
};
