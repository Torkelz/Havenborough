#include <boost/test/unit_test.hpp>

#include <CommandManager.h>
#include <StreamReader.h>

BOOST_AUTO_TEST_SUITE(TestStreamReader)

class StubCommand : public Command
{
public:
	std::vector<std::vector<std::string>> calls;

	const std::string& getName() const override
	{
		const static std::string name = "stub";
		return name;
	}
	void run(const std::vector<std::string>& p_Arguments) override
	{
		calls.push_back(p_Arguments);
	}
};

BOOST_AUTO_TEST_CASE(TestStreamReaderInput)
{
	std::string input("stub arg1\nstub arg1 arg2\nstub");
	std::istringstream stream(input);

	std::shared_ptr<StubCommand> command(new StubCommand);
	CommandManager::ptr manager(new CommandManager);
	manager->registerCommand(command);

	{
		StreamReader reader(manager, stream);

		reader.readAll();
		reader.handleInput();
	}

	BOOST_REQUIRE_EQUAL(command->calls.size(), 3);

	BOOST_REQUIRE_EQUAL(command->calls[0].size(), 2);
	BOOST_CHECK_EQUAL(command->calls[0][0], "stub");
	BOOST_CHECK_EQUAL(command->calls[0][1], "arg1");

	BOOST_REQUIRE_EQUAL(command->calls[1].size(), 3);
	BOOST_CHECK_EQUAL(command->calls[1][0], "stub");
	BOOST_CHECK_EQUAL(command->calls[1][1], "arg1");
	BOOST_CHECK_EQUAL(command->calls[1][2], "arg2");

	BOOST_REQUIRE_EQUAL(command->calls[2].size(), 1);
	BOOST_CHECK_EQUAL(command->calls[2][0], "stub");
}

BOOST_AUTO_TEST_SUITE_END()