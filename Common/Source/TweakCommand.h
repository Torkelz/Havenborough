#pragma once

#include "Command.h"
#include "TweakSettings.h"

class TweakCommand : public Command
{
public:
	static const std::string m_CommandName;

public:
	const std::string& getName() const override;
	void run(const std::vector<std::string>& p_Arguments) override;
};
