#pragma once

#include "Command.h"

#include <map>
#include <string>
#include <vector>

class CommandManager
{
public:
	typedef std::shared_ptr<CommandManager> ptr;

private:
	std::map<std::string, Command::ptr> m_Commands;

public:
	void registerCommand(Command::ptr p_Command);

	void runCommand(const std::string& p_CommandLine);
	void runCommand(const std::vector<std::string>& p_CommandArguments);
};
