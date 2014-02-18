#include <boost/test/unit_test.hpp>
#include "../../Client/Source/Settings.h"
#include "../../Client/Source/ClientExceptions.h"

BOOST_AUTO_TEST_SUITE(SettingsTest)

BOOST_AUTO_TEST_CASE(initializeSettingsRightFile)
{
	Settings settings;

	BOOST_CHECK_NO_THROW(settings.initialize("../../Client/Bin/UserOptions.xml"));
}
BOOST_AUTO_TEST_CASE(initializeSettingsWrongFile)
{
	Settings settings;

	BOOST_CHECK_THROW(settings.initialize("../../Client/Bin/UserOptions2.xml"), ClientException);
}
BOOST_AUTO_TEST_CASE(GetValidSetting)
{
	Settings settings;

	BOOST_CHECK_NO_THROW(settings.initialize("../../Client/Bin/UserOptions.xml"));

	BOOST_CHECK_NO_THROW(settings.getIsSettingEnabled("SSAO"));
}
BOOST_AUTO_TEST_CASE(GetNonValidSetting)
{
	Settings settings;

	BOOST_CHECK_NO_THROW(settings.initialize("../../Client/Bin/UserOptions.xml"));

	BOOST_CHECK_THROW(settings.getIsSettingEnabled("SSAOD"), ClientException);
}

BOOST_AUTO_TEST_SUITE_END()