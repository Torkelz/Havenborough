#include <boost/test/unit_test.hpp>
#include "../../Client/Source/Scenes/HUDScene.h"
#include "../../Client/Source/ClientExceptions.h"

BOOST_AUTO_TEST_SUITE(HUDSceneTest)

BOOST_AUTO_TEST_CASE(SetGet)
{
	HUDScene scene;

	scene.registeredInput("dd",0,0);

	BOOST_CHECK(scene.getIsVisible() == false);
	scene.setIsVisible(true);
	BOOST_CHECK(scene.getIsVisible() == true);
	BOOST_CHECK(scene.getID() == 0);
}

BOOST_AUTO_TEST_CASE(CreateElement)
{
	HUDScene scene;

	BOOST_CHECK_NO_THROW(scene.createGUIElementTest("d", 0));
	BOOST_CHECK_THROW(scene.createGUIElementTest("d", 0), SceneException);
	BOOST_CHECK_NO_THROW(scene.createTextElementTest("d", 0));
	BOOST_CHECK_THROW(scene.createTextElementTest("d", 0), SceneException);
}

BOOST_AUTO_TEST_SUITE_END()