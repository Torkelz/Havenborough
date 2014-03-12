#include <boost/test/unit_test.hpp>
#include "../../Client/Source/Settings.h"
#include "../../Client/Source/ClientExceptions.h"

BOOST_AUTO_TEST_SUITE(SettingsTest)

BOOST_AUTO_TEST_CASE(initializeSettingsRightFile)
{
	Settings settings;

	BOOST_CHECK_NO_THROW(settings.initialize("../Source/TestSettingsFiles/TestUserOptions.xml"));
}
BOOST_AUTO_TEST_CASE(initializeSettingsWrongFile)
{
	Settings settings;

	BOOST_CHECK_THROW(settings.initialize("../Source/TestSettingsFiles/TestUserOptions2.xml"), ClientException);
}
BOOST_AUTO_TEST_CASE(GetValidSetting)
{
	Settings settings;

	BOOST_CHECK_NO_THROW(settings.initialize("../Source/TestSettingsFiles/TestUserOptions.xml"));

	BOOST_CHECK_NO_THROW(settings.getIsSettingEnabled("SSAO"));
}
BOOST_AUTO_TEST_CASE(GetNonValidSetting)
{
	Settings settings;

	BOOST_CHECK_NO_THROW(settings.initialize("../Source/TestSettingsFiles/TestUserOptions.xml"));

	BOOST_CHECK_THROW(settings.getIsSettingEnabled("SSAOD"), ClientException);
}
BOOST_AUTO_TEST_CASE(ReturnSettingsLoadedFromFIle)
{
	Settings settings;

	Vector2 controlValue = settings.getResolution();

	BOOST_CHECK_NO_THROW(settings.initialize("../Source/TestSettingsFiles/TestUserOptions.xml"));

	BOOST_CHECK(settings.getKeyMap().size() > 0);
	BOOST_CHECK(settings.getMouseButtonMap().size() > 0);
	BOOST_CHECK(settings.getMouseMap().size() > 0);

	Vector2 retValue = settings.getResolution();
	BOOST_CHECK(retValue.x != controlValue.x);
	BOOST_CHECK(retValue.y != controlValue.y);
}
BOOST_AUTO_TEST_CASE(LoadingFileThrowsCheck)
{
	Settings settings;

	BOOST_CHECK_THROW(settings.initialize("../Source/TestSettingsFiles/TestUserOptionsWrongKey.xml"), ClientException);
	BOOST_CHECK_THROW(settings.initialize("../Source/TestSettingsFiles/TestUserOptionsWrongMouse.xml"), ClientException);
	BOOST_CHECK_THROW(settings.initialize("../Source/TestSettingsFiles/TestUserOptionsWrongMouseButton.xml"), ClientException);
	BOOST_CHECK_THROW(settings.initialize("../Source/TestSettingsFiles/TestUserOptionsWrongCommand.xml"), ClientException);
	BOOST_CHECK_THROW(settings.initialize("../Source/TestSettingsFiles/TestUserOptionsWrongKeyBind.xml"), ClientException);
}
BOOST_AUTO_TEST_SUITE_END()