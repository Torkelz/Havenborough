/**
 *
 */
#pragma once

#include "Command.h"
#include "TweakSettings.h"

/**
 * Command for changing tweakable settings during runtime.
 */
class TweakCommand : public Command
{
public:
	/**
	 * The name of the command.
	 */
	static const std::string m_CommandName;

private:
	static const std::string m_DefaultFile;

public:
	const std::string& getName() const override;
	void run(const std::vector<std::string>& p_Arguments) override;

private:
	void setSetting(const std::vector<std::string>& p_Arguments) const;
	void saveSettings(const std::vector<std::string>& p_Arguments) const;
	void loadSettings(const std::vector<std::string>& p_Arguments) const;
	void listSettings(const std::vector<std::string>& p_Arguments) const;
};
