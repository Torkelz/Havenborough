#include <boost/test/unit_test.hpp>

#include <CommonExceptions.h>
#include <Logger.h>
#include <TweakSettings.h>

#include <tinyxml2/tinyxml2.h>

BOOST_AUTO_TEST_SUITE(TestTweakSettings)

BOOST_AUTO_TEST_CASE(TestQueryTweakSettings)
{
	TweakSettings settings;

	static const float testFloat1 = 1234.5f;
	static const float testFloat2 = 345.6f;
	static const std::string testString1("TestString1");
	static const std::string testString2("TestString2");
	static const int testInt1 = 123;
	static const int testInt2 = 4321;
	static const bool testBool1 = true;
	static const bool testBool2 = false;

	float f = testFloat1;
	settings.querySetting("TestFloat", f);
	BOOST_CHECK_EQUAL(f, testFloat1);

	settings.setSetting("TestFloat", testFloat2);
	settings.querySetting("TestFloat", f);
	BOOST_CHECK_EQUAL(f, testFloat2);

	std::string str = testString1;
	settings.querySetting("TestString", str);
	BOOST_CHECK_EQUAL(str, testString1);

	settings.setSetting("TestString", testString2);
	settings.querySetting("TestString", str);
	BOOST_CHECK_EQUAL(str, testString2);

	int i = testInt1;
	settings.querySetting("TestInt", i);
	BOOST_CHECK_EQUAL(i, testInt1);

	settings.setSetting("TestInt", testInt2);
	settings.querySetting("TestInt", i);
	BOOST_CHECK_EQUAL(i, testInt2);

	bool b = testBool1;
	settings.querySetting("TestBool", b);
	BOOST_CHECK_EQUAL(b, testBool1);

	settings.setSetting("TestBool", testBool2);
	settings.querySetting("TestBool", b);
	BOOST_CHECK_EQUAL(b, testBool2);

	settings.setSetting("TestBool", testBool1);
	settings.querySetting("TestBool", b);
	BOOST_CHECK_EQUAL(b, testBool1);

	BOOST_CHECK_EQUAL(f, testFloat2);
	BOOST_CHECK_EQUAL(str, testString2);
	BOOST_CHECK_EQUAL(i, testInt2);
}

BOOST_AUTO_TEST_CASE(TestTweakSettingsListener)
{
	TweakSettings settings;

	static const float testFloat1 = 123.4f;
	static const float testFloat2 = 34.5f;
	static const std::string testString1("TestString1");
	static const std::string testString2("TestString2");
	static const int testInt1 = 567;
	static const int testInt2 = 341;
	static const bool testBool1 = true;
	static const bool testBool2 = false;

	float f1 = testFloat1;
	float f2 = testFloat1;
	settings.setListener(std::string("TestFloat1"), std::function<void(float)>([&] (float p_F) { f1 = p_F; }));
	settings.setListener(std::string("TestFloat2"), std::function<void(float)>([&] (float p_F) { f2 = p_F; }));
	settings.setSetting("TestFloat1", testFloat2);
	BOOST_CHECK_EQUAL(f1, testFloat2);
	BOOST_CHECK_EQUAL(f2, testFloat1);

	std::string str = testString1;
	settings.setListener(std::string("TestString"), std::function<void(const std::string&)>([&] (const std::string& p_Val) { str = p_Val; }));
	settings.setSetting("TestString", testString2);
	BOOST_CHECK_EQUAL(str, testString2);

	int i = testInt1;
	settings.setListener(std::string("TestInt"), std::function<void(int)>([&] (int p_Val) { i = p_Val; }));
	settings.setSetting("TestInt", testInt2);
	BOOST_CHECK_EQUAL(i, testInt2);

	bool b = testBool1;
	settings.setListener("TestBool", std::function<void(bool)>([&] (bool p_Val) { b = p_Val; }));
	settings.setSetting("TestBool", testBool2);
	BOOST_CHECK_EQUAL(b, testBool2);

	settings.setSetting("TestBool", testBool1);
	BOOST_CHECK_EQUAL(b, testBool1);
}

BOOST_AUTO_TEST_CASE(TestTweakSettingsSingleton)
{
	TweakSettings::initializeMaster();
	TweakSettings* settings1 = TweakSettings::getInstance();
	TweakSettings* settings2 = TweakSettings::getInstance();
	BOOST_CHECK_EQUAL(settings1, settings2);
	BOOST_CHECK_NE(settings1, (TweakSettings*)nullptr);

	static const int testVal1 = 1337;
	static const int testVal2 = 0;

	settings1->setSetting("TestSetting", testVal1);
	TweakSettings::shutdown();
	TweakSettings::initializeMaster();
	settings1 = TweakSettings::getInstance();
	
	int val = testVal2;
	settings1->querySetting("TestSetting", val);
	BOOST_CHECK_EQUAL(val, testVal2);

	TweakSettings::shutdown();
}

BOOST_AUTO_TEST_CASE(TestTweakSettingsListenerException)
{
	std::ostringstream testOut;

	Logger::reset();
	Logger::addOutput(Logger::Level::ALL, testOut);

	TweakSettings settings;
	settings.setListener("TestSetting", std::function<void(const std::string&)>(
		[] (const std::string&)
		{
			throw std::exception("Test exception");
		}
	));

	settings.setSetting("TestSetting", std::string("TestValue"));

	Logger::reset();

	std::string logOutput = testOut.str();
	BOOST_CHECK_NE(logOutput.find("WARNING"), std::string::npos);
	BOOST_CHECK_NE(logOutput.find("Test exception"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(TestTweakSettingsInitializeException)
{
	BOOST_CHECK_THROW(TweakSettings::getInstance(), CommonException)
}

BOOST_AUTO_TEST_CASE(TestTweakSettingsDoubleInitialize)
{
	TweakSettings::initializeMaster();
	TweakSettings* master1 = TweakSettings::getInstance();

	TweakSettings::initializeMaster();
	TweakSettings* master2 = TweakSettings::getInstance();

	BOOST_CHECK_NE(master1, master2);

	TweakSettings::shutdown();
}

BOOST_AUTO_TEST_CASE(TestTweakSettingsInitializeSlave)
{
	TweakSettings settings;

	TweakSettings::initializeSlave(&settings);

	TweakSettings* slave = TweakSettings::getInstance();
	BOOST_REQUIRE(slave);

	static const int testVal = 123;
	slave->setSetting("TestSetting", testVal);

	int setVal = -1;
	settings.querySetting("TestSetting", setVal);
	BOOST_CHECK_EQUAL(setVal, testVal);

	TweakSettings::shutdown();
}

BOOST_AUTO_TEST_CASE(TestTweakSettingsLoad)
{
	TweakSettings settings;

	static const char* validSettings(
		"<TweakSettings>"
		"	<Setting Name=\"TestSetting\" Value=\"123\" />"
		"</TweakSettings>"
		);

	std::istringstream validXML(validSettings);
	BOOST_CHECK_NO_THROW(settings.loadSettings(validXML));

	int setVal = -1;
	settings.querySetting("TestSetting", setVal);
	BOOST_CHECK_EQUAL(setVal, 123);

	BOOST_CHECK_THROW(settings.loadSettings(std::istringstream()), InvalidArgument);

	static const char* invalidSettings(
		"<InvalidRootElement />"
		);

	BOOST_CHECK_THROW(settings.loadSettings(std::istringstream(invalidSettings)), InvalidArgument);
}

BOOST_AUTO_TEST_CASE(TestTweakSettingsSave)
{
	TweakSettings settings;

	settings.setSetting("TestSetting", std::string("TestValue"));

	std::ostringstream output;
	settings.saveSettings(output);

	std::string outputString(output.str());

	tinyxml2::XMLDocument doc;
	BOOST_REQUIRE_EQUAL(doc.Parse(outputString.c_str(), outputString.size()), tinyxml2::XML_NO_ERROR);

	const tinyxml2::XMLElement* root = doc.FirstChildElement();
	BOOST_REQUIRE(root);
	BOOST_CHECK_EQUAL(root->Name(), "TweakSettings");

	const tinyxml2::XMLElement* setting = root->FirstChildElement("Setting");
	BOOST_REQUIRE(setting);
	BOOST_CHECK(setting->Attribute("Name", "TestSetting"));
	BOOST_CHECK(setting->Attribute("Value", "TestValue"));
}

BOOST_AUTO_TEST_CASE(TestTweakSettingsGetSettings)
{
	TweakSettings settings;

	settings.setSetting("TestSetting1", std::string("TestValue"));
	settings.setSetting("TestSetting2", 123);
	settings.setSetting("TestSetting3", true);
	settings.setSetting("TestSetting4", false);

	std::vector<std::string> descs = settings.getSettings();
	BOOST_REQUIRE_EQUAL(descs.size(), 4);
	BOOST_CHECK_EQUAL(descs[0], "TestSetting1 = TestValue");
	BOOST_CHECK_EQUAL(descs[1], "TestSetting2 = 123");
	BOOST_CHECK_EQUAL(descs[2], "TestSetting3 = true");
	BOOST_CHECK_EQUAL(descs[3], "TestSetting4 = false");
}

BOOST_AUTO_TEST_SUITE_END()
