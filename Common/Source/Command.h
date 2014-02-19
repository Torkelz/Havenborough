#pragma once

#include <memory>
#include <vector>

class Command
{
public:
	typedef std::shared_ptr<Command> ptr;

	~Command() {};

	virtual const std::string& getName() const = 0;
	virtual void run(const std::vector<std::string>& p_Arguments) = 0;
};
