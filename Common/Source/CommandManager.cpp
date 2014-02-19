#include "CommandManager.h"

#include "CommonExceptions.h"
#include "Logger.h"

#include <sstream>

void CommandManager::registerCommand(Command::ptr p_Command)
{
	m_Commands[p_Command->getName()] = p_Command;
}

void CommandManager::runCommand(const std::string& p_CommandLine)
{
	std::vector<std::string> arguments;

	std::istringstream stream(p_CommandLine);
	std::string arg;
	while (stream >> arg)
	{
		arguments.push_back(arg);
	}

	runCommand(arguments);
}

void CommandManager::runCommand(const std::vector<std::string>& p_CommandArguments)
{
	if (p_CommandArguments.empty())
	{
		throw InvalidArgument("A command must have at least one argument (the command name).", __LINE__, __FILE__);
	}

	auto commandIt = m_Commands.find(p_CommandArguments[0]);
	if (commandIt == m_Commands.end())
	{
		Logger::log(Logger::Level::INFO, "Command '" + p_CommandArguments[0] + "' does not exist.");
		return;
	}

	commandIt->second->run(p_CommandArguments);
}
