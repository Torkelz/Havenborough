/**
 *
 */

#pragma once

#include <memory>
#include <vector>

/**
 * Interface for text commands, for example from a console.
 */
class Command
{
public:
	/**
	 * Smart pointer type.
	 */
	typedef std::shared_ptr<Command> ptr;

	/**
	 * destructor.
	 */
	~Command() {};

	/*
	 * Get the name of the command.
	 *
	 * The name of the command is the text that is used to execute said command.
	 * The name should be used without regards to case during comparisons.
	 *
	 * The name of the command may contain valid ascii characters,
	 * lowercase and uppercase letters, and numbers.
	 *
	 * @return the name of the command
	 */
	virtual const std::string& getName() const = 0;

	/**
	 * Run the command, with any number of extra arguments.
	 *
	 * The first arguments should always be the command name used to run the command,
	 * and may be the name returned by {@link #getName} or an alias.
	 *
	 * @param p_Arguments the arguments to send to the command
	 */
	virtual void run(const std::vector<std::string>& p_Arguments) = 0;
};
