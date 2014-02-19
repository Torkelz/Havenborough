#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

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
	static TweakSettings* getInstance();

	static void initializeMaster();
	static void initializeSlave(TweakSettings* p_Master);
	static void shutdown();

	void saveSettings(std::ostream& p_Out) const;
	void loadSettings(std::istream& p_In);

	std::vector<std::string> getSettings() const;

	void querySetting(const std::string& p_Setting, std::string& p_OutVal) const;
	void querySetting(const std::string& p_Setting, float& p_OutVal) const;
	void querySetting(const std::string& p_Setting, int& p_OutVal) const;
	void querySetting(const std::string& p_Setting, bool& p_OutVal) const;

	void setSetting(const std::string& p_Setting, const std::string& p_Val);
	void setSetting(const std::string& p_Setting, float p_Val);
	void setSetting(const std::string& p_Setting, int p_Val);
	void setSetting(const std::string& p_Setting, bool p_Val);

	void setListener(const std::string& p_Setting, std::function<void(const std::string&)> p_Listener);
	void setListener(const std::string& p_Setting, std::function<void(float)> p_Listener);
	void setListener(const std::string& p_Setting, std::function<void(int)> p_Listener);
	void setListener(const std::string& p_Setting, std::function<void(bool)> p_Listener);
};
