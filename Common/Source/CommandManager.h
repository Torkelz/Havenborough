/**
 *
 */
#pragma once

#include "Command.h"

#include <map>
#include <string>
#include <vector>

/**
 * The CommandManager is responsible for keeping track of commands
 * and running the appropriate one when told to.
 */
class CommandManager
{
public:
	/**
	 * Smart pointer type.
	 */
	typedef std::shared_ptr<CommandManager> ptr;

private:
	std::map<std::string, Command::ptr> m_Commands;

public:
	/**
	 * Registers a new command, to eventually be called later.
	 *
	 * If the command already exists, the old command is overwritten with the new one.
	 *
	 * @param p_Command the command to add
	 * @throws InvalidArgument if p_Command is empty
	 */
	void registerCommand(Command::ptr p_Command);

	/**
	 * Runs a command described as a single string.
	 *
	 * The format of the string is 'command [arg1 ...], where command is the
	 * name of the command or an alias and is optionally
	 * followed by any number of arguments separated by white-space.
	 * White-space in command names or arguments are currently not supported.
	 *
	 * @param p_CommandLine a string containing the command and any arguments
	 * @throws InvalidArgument if the string is empty
	 */
	void runCommand(const std::string& p_CommandLine);
	/**
	 * Runs a command with any number of arguments.
	 *
	 * The first arguments is the name of the command or an alias,
	 * optionally followed by any number of arguments.
	 * The command name and the arguments may contain white-space or any other valid charater.
	 *
	 * @param p_CommandArguments the command and arguments
	 * @throws InvalidArgument if no command is passed a the function name
	 */
	void runCommand(const std::vector<std::string>& p_CommandArguments);
};
