#include <boost/test/unit_test.hpp>
#include <Logger.h>

BOOST_AUTO_TEST_SUITE(LoggerTest)

BOOST_AUTO_TEST_CASE(TestLogging)
{
	std::ostringstream testStream;
	Logger::addOutput(Logger::Level::ALL, testStream);

	BOOST_CHECK_EQUAL(testStream.str(), "");

	Logger::log(Logger::Level::INFO, "Test output");

	BOOST_CHECK_NE(testStream.str().find("INFO: Test output\n"), std::string::npos);

	Logger::reset();
}

BOOST_AUTO_TEST_CASE(TestLogLevel)
{
	std::ostringstream testStream;
	Logger::addOutput(Logger::Level::INFO, testStream);

	Logger::log(Logger::Level::DEBUG_L, "Debug output");

	BOOST_CHECK_EQUAL(testStream.str().find("Debug output"), std::string::npos);

	Logger::log(Logger::Level::INFO, "Info output");
	
	BOOST_CHECK_EQUAL(testStream.str().find("Debug output"), std::string::npos);
	BOOST_CHECK_NE(testStream.str().find("Info output"), std::string::npos);

	Logger::log(Logger::Level::WARNING, "Warning output");
	
	BOOST_CHECK_EQUAL(testStream.str().find("Debug output"), std::string::npos);
	BOOST_CHECK_NE(testStream.str().find("Info output"), std::string::npos);
	BOOST_CHECK_NE(testStream.str().find("Warning output"), std::string::npos);

	Logger::reset();
}

BOOST_AUTO_TEST_SUITE_END()
