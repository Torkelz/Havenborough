#include <boost/test/unit_test.hpp>

#include <Logger.h>
#include <TweakCommand.h>

BOOST_AUTO_TEST_SUITE(TestTweakCommand)

BOOST_AUTO_TEST_CASE(TestTweakCommandRun)
{
	std::ostringstream outStream;

	Logger::reset();
	Logger::addOutput(Logger::Level::ALL, outStream);

	TweakSettings::initializeMaster();
	TweakSettings::getInstance()->setSetting("TestSetting", 12345);

	TweakCommand command;

	BOOST_CHECK_EQUAL(command.getName(), TweakCommand::m_CommandName);

	std::vector<std::string> args;

	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("To few arguments"), std::string::npos);
	outStream.clear();

	args.push_back("randomCommandName");
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("To few arguments"), std::string::npos);
	outStream.clear();

	args.push_back("invalidSubcommand");
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("Unknown argument"), std::string::npos);
	outStream.clear();

	TweakSettings::shutdown();

	Logger::reset();
}

BOOST_AUTO_TEST_CASE(TestTweakCommandSet)
{
	std::ostringstream outStream;

	Logger::reset();
	Logger::addOutput(Logger::Level::ALL, outStream);

	TweakSettings::initializeMaster();

	TweakCommand command;

	std::vector<std::string> args;
	args.push_back("randomCommandName");
	args.push_back("set");

	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("Invalid"), std::string::npos);
	outStream.clear();

	args.push_back("TestSetting");
	args.push_back("TestValue");
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("Set"), std::string::npos);
	outStream.clear();

	std::string setVal;
	TweakSettings::getInstance()->querySetting("TestSetting", setVal);
	BOOST_CHECK_EQUAL(setVal, "TestValue");

	TweakSettings::shutdown();

	Logger::reset();
}

BOOST_AUTO_TEST_CASE(TestTweakCommandSave)
{
	std::ostringstream outStream;

	Logger::reset();
	Logger::addOutput(Logger::Level::ALL, outStream);

	TweakSettings::initializeMaster();

	TweakCommand command;

	std::vector<std::string> args;
	args.push_back("randomCommandName");
	args.push_back("SAve");

	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("Saved settings"), std::string::npos);
	outStream.clear();

	args.push_back("testFile.tmp");
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("testFile.tmp"), std::string::npos);
	outStream.clear();

	args[2] = "invalidPath ?!)=@£$$£€***!";
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("Failed"), std::string::npos);
	outStream.clear();

	args.push_back("anotherArgument");
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("Invalid number"), std::string::npos);
	outStream.clear();

	TweakSettings::shutdown();

	Logger::reset();
}

BOOST_AUTO_TEST_CASE(TestTweakCommandLoad)
{
	std::ostringstream outStream;

	Logger::reset();
	Logger::addOutput(Logger::Level::ALL, outStream);

	TweakSettings::initializeMaster();

	TweakCommand command;

	std::vector<std::string> args;
	args.push_back("randomCommandName");
	args.push_back("load");

	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("Loaded settings"), std::string::npos);
	outStream.clear();

	args.push_back("tweakSettings.xml");
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("tweakSettings.xml"), std::string::npos);
	outStream.clear();

	args[2] = "invalidPath ?!)=@£$$£€***!";
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("Failed"), std::string::npos);
	outStream.clear();

	args.push_back("anotherArgument");
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("Invalid number"), std::string::npos);
	outStream.clear();

	TweakSettings::shutdown();

	Logger::reset();
}

BOOST_AUTO_TEST_CASE(TestTweakCommandList)
{
	std::ostringstream outStream;

	Logger::reset();
	Logger::addOutput(Logger::Level::ALL, outStream);

	TweakSettings::initializeMaster();

	TweakCommand command;

	std::vector<std::string> args;
	args.push_back("randomCommandName");
	args.push_back("list");

	command.run(args);
	BOOST_CHECK_EQUAL(outStream.str().size(), 0);
	outStream.clear();

	TweakSettings::getInstance()->setSetting("TestSetting", std::string("TestValue"));

	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("TestValue"), std::string::npos);
	outStream.clear();

	args.push_back("TestSetting");
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("TestValue"), std::string::npos);
	outStream.clear();

	args.push_back("anotherArgument");
	command.run(args);
	BOOST_CHECK_NE(outStream.str().find("Invalid number"), std::string::npos);
	outStream.clear();

	TweakSettings::shutdown();

	Logger::reset();
}

BOOST_AUTO_TEST_SUITE_END()
