#define BOOST_TEST_MODULE "Unit tests for Havenborough"
#include <boost/test/unit_test.hpp>
#include "..\..\Client\Source\SceneManager.h"

/**
 * Place tests in separate files. Look at TestWindow for example.
 */

#pragma region SceneManager
/*TEST NOTES*/
/**
* Testing the Scene Manager and its key components.
* Notifications:
* 1 in testOnFrame will test the change scene element.
* 2 in testOnFrame will test the change list element.
* 3 in testOnFrame Will test the pause menu in the run list.
* 1 in testRender will test menu rendering; true/false = with/without change scene.
* 2 in testRender will test game rendering; true/false = with/without change scene.
*/

/*For this test run the settings on 1*/
BOOST_AUTO_TEST_CASE(ChangeScene)
{
	SceneManager Scenes;
	int setting = 1;
	BOOST_CHECK_EQUAL(Scenes.init(),true);
	std::vector<IScene::ptr> Tested;
	Tested = Scenes.testOnFrame(setting);
	int size = Tested.size();
	BOOST_REQUIRE_EQUAL(size, 1);
	int ID = Tested[0]->getID();
	BOOST_CHECK_EQUAL(ID, 1);
	int active = Tested[0]->getIsVisible();
	BOOST_CHECK_EQUAL(active,1);
	
	Tested = Scenes.testOnFrame(setting);
	size = Tested.size();
	BOOST_REQUIRE_EQUAL(size, 1);
	ID = Tested[0]->getID();
	BOOST_CHECK_EQUAL(ID, 0);
	active = Tested[0]->getIsVisible();
	BOOST_CHECK_EQUAL(active,1);
}

/*For this test run the settings on 2*/
BOOST_AUTO_TEST_CASE(ChangeListOfScenes)
{
	SceneManager Scenes;
	int setting = 2;
	BOOST_CHECK_EQUAL(Scenes.init(),true);
	std::vector<IScene::ptr> Tested;
	Tested = Scenes.testOnFrame(setting);
	int size = Tested.size();
	BOOST_REQUIRE_EQUAL(size, 2);
	int active = Tested[0]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	int ID = Tested[0]->getID();
	BOOST_CHECK_EQUAL(ID, 0);
	active = Tested[1]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	ID = Tested[1]->getID();
	BOOST_CHECK_EQUAL(ID, 1);
	
	Tested = Scenes.testOnFrame(setting);
	size = Tested.size();
	BOOST_REQUIRE_EQUAL(size, 1);
	active = Tested[0]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	ID = Tested[0]->getID();
	BOOST_CHECK_EQUAL(ID, 0);
}

/*For this test run the settings on 3*/
BOOST_AUTO_TEST_CASE(testPaused)
{
	SceneManager Scenes;
	int setting = 3;
	BOOST_CHECK_EQUAL(Scenes.init(),true);
	std::vector<IScene::ptr> Tested;
	Tested = Scenes.testOnFrame(setting);
	int size = Tested.size();
	BOOST_REQUIRE_EQUAL(size, 2);
	int active = Tested[0]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	int ID = Tested[0]->getID();
	BOOST_CHECK_EQUAL(ID, 0);
	active = Tested[1]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	ID = Tested[1]->getID();
	BOOST_CHECK_EQUAL(ID,1);

	Scenes.setPause();
	Tested = Scenes.getScene();
	size = Tested.size();
	BOOST_REQUIRE_EQUAL(size, 3);
	active = Tested[0]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	ID = Tested[0]->getID();
	BOOST_CHECK_EQUAL(ID, 0);
	active = Tested[1]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	ID = Tested[1]->getID();
	BOOST_CHECK_EQUAL(ID, 1);
	active = Tested[2]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	ID = Tested[2]->getID();
	BOOST_CHECK_EQUAL(ID, 2);

	Scenes.setPause();
	Tested = Scenes.getScene();
	size = Tested.size();
	BOOST_REQUIRE_EQUAL(size, 2);
	active = Tested[0]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	ID = Tested[0]->getID();
	BOOST_CHECK_EQUAL(ID, 0);
	active = Tested[1]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	ID = Tested[1]->getID();
	BOOST_CHECK_EQUAL(ID,1);
}

#pragma endregion
