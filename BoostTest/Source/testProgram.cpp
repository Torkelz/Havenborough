#define BOOST_TEST_MODULE "Unit tests for Havenborough"
#include <boost/test/unit_test.hpp>
#include "..\..\Client\Source\SceneManager.h"

/**
 * Place tests in separate files. Look at TestWindow for example.
 */

#pragma region SceneManager
BOOST_AUTO_TEST_CASE(ChangeScene)
{
	SceneManager Scenes;
	BOOST_CHECK_EQUAL(Scenes.init(),true);
	std::vector<IScene::ptr> Tested;
	LRESULT res;
	Scenes.keyStroke('L', NULL, res);
	Scenes.onFrame();
	Tested = Scenes.getScene();
	int size = Tested.size();
	BOOST_REQUIRE_EQUAL(size, 1);
	int ID = Tested[0]->getID();
	BOOST_CHECK_EQUAL(ID, 1);
	int active = Tested[0]->getIsVisible();
	BOOST_CHECK_EQUAL(active,1);
	Scenes.keyStroke('L', NULL, res);
	Scenes.onFrame();
	Tested = Scenes.getScene();
	size = Tested.size();
	BOOST_REQUIRE_EQUAL(size, 1);
	ID = Tested[0]->getID();
	BOOST_CHECK_EQUAL(ID, 0);
	active = Tested[0]->getIsVisible();
	BOOST_CHECK_EQUAL(active,1);
}

BOOST_AUTO_TEST_CASE(ChangeListOfScenes)
{
	SceneManager Scenes;
	BOOST_CHECK_EQUAL(Scenes.init(),true);
	std::vector<IScene::ptr> Tested;
	LRESULT res;
	Scenes.keyStroke('J', NULL, res);
	Scenes.onFrame();
	Tested = Scenes.getScene();
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

	Scenes.keyStroke('J', NULL, res);
	Scenes.onFrame();
	Tested = Scenes.getScene();
	size = Tested.size();
	BOOST_REQUIRE_EQUAL(size, 1);
	active = Tested[0]->getIsVisible();
	BOOST_CHECK_EQUAL(active, 1);
	ID = Tested[0]->getID();
	BOOST_CHECK_EQUAL(ID, 0);
}
//
///*For this test run the settings on 3*/
//BOOST_AUTO_TEST_CASE(testPaused)
//{
//	//SceneManager Scenes;
//	//BOOST_CHECK_EQUAL(Scenes.init(),true);
//	//
//	//int size = Tested.size();
//	//BOOST_REQUIRE_EQUAL(size, 2);
//	//int active = Tested[0]->getIsVisible();
//	//BOOST_CHECK_EQUAL(active, 1);
//	//int ID = Tested[0]->getID();
//	//BOOST_CHECK_EQUAL(ID, 0);
//	//active = Tested[1]->getIsVisible();
//	//BOOST_CHECK_EQUAL(active, 1);
//	//ID = Tested[1]->getID();
//	//BOOST_CHECK_EQUAL(ID,1);
//	/*
//	Scenes.setPause();
//	Tested = Scenes.getScene();
//	size = Tested.size();
//	BOOST_REQUIRE_EQUAL(size, 3);
//	active = Tested[0]->getIsVisible();
//	BOOST_CHECK_EQUAL(active, 1);
//	ID = Tested[0]->getID();
//	BOOST_CHECK_EQUAL(ID, 0);
//	active = Tested[1]->getIsVisible();
//	BOOST_CHECK_EQUAL(active, 1);
//	ID = Tested[1]->getID();
//	BOOST_CHECK_EQUAL(ID, 1);
//	active = Tested[2]->getIsVisible();
//	BOOST_CHECK_EQUAL(active, 1);
//	ID = Tested[2]->getID();
//	BOOST_CHECK_EQUAL(ID, 2);
//
//	Scenes.setPause();
//	Tested = Scenes.getScene();
//	size = Tested.size();
//	BOOST_REQUIRE_EQUAL(size, 2);
//	active = Tested[0]->getIsVisible();
//	BOOST_CHECK_EQUAL(active, 1);
//	ID = Tested[0]->getID();
//	BOOST_CHECK_EQUAL(ID, 0);
//	active = Tested[1]->getIsVisible();
//	BOOST_CHECK_EQUAL(active, 1);
//	ID = Tested[1]->getID();
//	BOOST_CHECK_EQUAL(ID,1);*/
//}

#pragma endregion
