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

public:
	const std::string& getName() const override;
	void run(const std::vector<std::string>& p_Arguments) override;
};
