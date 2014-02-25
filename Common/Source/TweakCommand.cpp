#include "TweakCommand.h"

#include "CommonExceptions.h"
#include "Logger.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <fstream>

const std::string TweakCommand::m_CommandName("tweak");
const std::string TweakCommand::m_DefaultFile("tweakSettings.xml");

const std::string& TweakCommand::getName() const
{
	return m_CommandName;
}

void TweakCommand::run(const std::vector<std::string>& p_Arguments)
{
	if (p_Arguments.size() < 2)
	{
		Logger::log(Logger::Level::INFO, "To few arguments to command");
		return;
	}

	if (boost::iequals(p_Arguments[1], "set"))
	{
		setSetting(p_Arguments);
	}
	else if (boost::iequals(p_Arguments[1], "save"))
	{
		saveSettings(p_Arguments);
	}
	else if (boost::iequals(p_Arguments[1], "load"))
	{
		loadSettings(p_Arguments);
	}
	else if (boost::iequals(p_Arguments[1], "list"))
	{
		listSettings(p_Arguments);
	}
	else
	{
		Logger::log(Logger::Level::INFO, "Unknown argument: '" + p_Arguments[1] + "'");
	}
}

void TweakCommand::setSetting(const std::vector<std::string>& p_Arguments) const
{
	if (p_Arguments.size() != 4)
	{
		Logger::log(Logger::Level::INFO, "Invalid number of arguments to command");
		return;
	}

	TweakSettings* settings = TweakSettings::getInstance();

	settings->setSetting(p_Arguments[2], p_Arguments[3]);
	Logger::log(Logger::Level::INFO, "Set '" + p_Arguments[2] + "' to '" + p_Arguments[3] + "'");
}

void TweakCommand::saveSettings(const std::vector<std::string>& p_Arguments) const
{
	if (p_Arguments.size() > 3)
	{
		Logger::log(Logger::Level::INFO, "Invalid number of arguments to command");
		return;
	}

	boost::filesystem::path filepath(m_DefaultFile);

	if (p_Arguments.size() == 3)
	{
		filepath = p_Arguments[2];
	}

	filepath = boost::filesystem::absolute(filepath);

	std::ofstream out(filepath.string());
	if (out)
	{
		TweakSettings::getInstance()->saveSettings(out);
		Logger::log(Logger::Level::INFO, "Saved settings to '" + filepath.string() + "'");
	}
	else
	{
		Logger::log(Logger::Level::WARNING, "Failed to save settings to '" + filepath.string() + "'");
	}
}

void TweakCommand::loadSettings(const std::vector<std::string>& p_Arguments) const
{
	if (p_Arguments.size() > 3)
	{
		Logger::log(Logger::Level::INFO, "Invalid number of arguments to command");
		return;
	}

	boost::filesystem::path filepath(m_DefaultFile);

	if (p_Arguments.size() == 3)
	{
		filepath = p_Arguments[2];
	}

	filepath = boost::filesystem::absolute(filepath);

	std::ifstream in(filepath.string());
	if (in)
	{
		try
		{
			TweakSettings::getInstance()->loadSettings(in);
			Logger::log(Logger::Level::INFO, "Loaded settings from '" + filepath.string() + "'");
		}
		catch (const InvalidArgument& p_Ex)
		{
			Logger::log(Logger::Level::WARNING, "Failed to load settings from '" + filepath.string() + "': " + p_Ex.what());
		}
	}
	else
	{
		Logger::log(Logger::Level::WARNING, "Failed to load settings from '" + filepath.string() + "'");
	}
}

void TweakCommand::listSettings(const std::vector<std::string>& p_Arguments) const
{
	if (p_Arguments.size() == 2)
	{
		for (const std::string& setting : TweakSettings::getInstance()->getSettings())
		{
			Logger::log(Logger::Level::INFO, setting);
		}
	}
	else if (p_Arguments.size() == 3)
	{
		std::string val = "Not set";
		TweakSettings::getInstance()->querySetting(p_Arguments[2], val);
		Logger::log(Logger::Level::INFO, p_Arguments[2] + " = " + val);
	}
	else
	{
		Logger::log(Logger::Level::INFO, "Invalid number of arguments to command");
	}
}
