#include <boost/test/unit_test.hpp>

#include <CommandManager.h>
#include <CommonExceptions.h>
#include <Logger.h>

BOOST_AUTO_TEST_SUITE(TestCommandManager)

class StubCommand : public Command
{
public:
	std::vector<std::string> latestArguments;

	const std::string& getName() const override
	{
		const static std::string name = "stub";
		return name;
	}
	void run(const std::vector<std::string>& p_Arguments) override
	{
		latestArguments = p_Arguments;
	}
};

BOOST_AUTO_TEST_CASE(TestCommandManagerRegister)
{
	CommandManager manager;

	std::shared_ptr<StubCommand> command(new StubCommand);
	BOOST_CHECK_NO_THROW(manager.registerCommand(command));

	BOOST_CHECK_THROW(manager.registerCommand(Command::ptr()), InvalidArgument);
}

BOOST_AUTO_TEST_CASE(TestCommandManagerRunLine)
{
	CommandManager manager;

	std::shared_ptr<StubCommand> command(new StubCommand);
	manager.registerCommand(command);

	manager.runCommand(command->getName());
	BOOST_REQUIRE_EQUAL(command->latestArguments.size(), 1);
	BOOST_CHECK_EQUAL(command->latestArguments[0], command->getName());

	manager.runCommand("STUB");
	BOOST_REQUIRE_EQUAL(command->latestArguments.size(), 1);
	BOOST_CHECK_EQUAL(command->latestArguments[0], "STUB");

	manager.runCommand("stub arg1");
	BOOST_REQUIRE_EQUAL(command->latestArguments.size(), 2);
	BOOST_CHECK_EQUAL(command->latestArguments[0], "stub");
	BOOST_CHECK_EQUAL(command->latestArguments[1], "arg1");
}

BOOST_AUTO_TEST_CASE(TestCommandManagerRunArgs)
{
	std::ostringstream output;

	Logger::reset();
	Logger::addOutput(Logger::Level::ALL, output);

	CommandManager manager;

	std::shared_ptr<StubCommand> command(new StubCommand);
	manager.registerCommand(command);

	std::vector<std::string> args;
	BOOST_CHECK_THROW(manager.runCommand(args), InvalidArgument);

	args.push_back("non-existingCommand");
	manager.runCommand(args);
	BOOST_CHECK_NE(output.str().find("does not exist"), std::string::npos);

	Logger::reset();
}

BOOST_AUTO_TEST_SUITE_END()