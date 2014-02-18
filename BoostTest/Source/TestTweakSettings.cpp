#include <boost/test/unit_test.hpp>
#include <TweakSettings.h>

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
}

BOOST_AUTO_TEST_CASE(TestTweakSettingsSingleton)
{
	TweakSettings* settings1 = TweakSettings::getInstance();
	TweakSettings* settings2 = TweakSettings::getInstance();
	BOOST_CHECK_EQUAL(settings1, settings2);
	BOOST_CHECK_NE(settings1, (TweakSettings*)nullptr);

	static const int testVal1 = 1337;
	static const int testVal2 = 0;

	settings1->setSetting("TestSetting", testVal1);
	TweakSettings::shutdown();
	settings1 = TweakSettings::getInstance();
	
	int val = testVal2;
	settings1->querySetting("TestSetting", val);
	BOOST_CHECK_EQUAL(val, testVal2);
}

BOOST_AUTO_TEST_SUITE_END()
