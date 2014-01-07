//#include <boost/test/unit_test.hpp>
//#include "..\..\Client\Source\SceneManager.h"
//
//BOOST_AUTO_TEST_SUITE(SceneManagerTest)
//
//
///**
//* This test changes scene to option menu and then goes back to menu again.
//*/
//BOOST_AUTO_TEST_CASE(ChangeScene)
//{
//	SceneManager Scenes;
//	BOOST_CHECK_EQUAL(Scenes.init(nullptr, nullptr, nullptr, nullptr),true);
//	std::vector<IScene::ptr> Tested;
//
//	//Simulate a keystroke.
//	Scenes.keyStroke("L", 0.f);
//	float dt = 0.f;
//	Scenes.onFrame(dt);
//	Tested = Scenes.getScene();
//	
//	//Check so that the right scene is shown.
//	int size = Tested.size();
//	BOOST_REQUIRE_EQUAL(size, 1);
//	int ID = Tested[0]->getID();
//	BOOST_CHECK_EQUAL(ID, 1);
//	int active = Tested[0]->getIsVisible();
//	BOOST_CHECK_EQUAL(active,1);
//
//	//Same as above but backwards.
//	Scenes.keyStroke("L", 0.f);
//	Scenes.onFrame(dt);
//	Tested = Scenes.getScene();
//	size = Tested.size();
//	BOOST_REQUIRE_EQUAL(size, 1);
//	ID = Tested[0]->getID();
//	BOOST_CHECK_EQUAL(ID, 0);
//	active = Tested[0]->getIsVisible();
//	BOOST_CHECK_EQUAL(active,1);
//}
//
//BOOST_AUTO_TEST_CASE(ChangeListOfScenes)
//{
//	SceneManager Scenes;
//	BOOST_CHECK_EQUAL(Scenes.init(nullptr, nullptr, nullptr, nullptr),true);
//	std::vector<IScene::ptr> Tested;
//
//	//Simulate a keystroke.
//	Scenes.keyStroke("J", 0.f);
//	float dt = 0.f;
//	Scenes.onFrame(dt);
//	Tested = Scenes.getScene();
//
//	//Check if the list has changed to game list
//	int size = Tested.size();
//	BOOST_REQUIRE_EQUAL(size, 2);
//	int active = Tested[0]->getIsVisible();
//	BOOST_CHECK_EQUAL(active, 1);
//	int ID = Tested[0]->getID();
//	BOOST_CHECK_EQUAL(ID, 0);
//	active = Tested[1]->getIsVisible();
//	BOOST_CHECK_EQUAL(active, 1);
//	ID = Tested[1]->getID();
//	BOOST_CHECK_EQUAL(ID, 1);
//
//	//Same as above but backwards.
//	Scenes.keyStroke("J", 0.f);
//	Scenes.onFrame(dt);
//	Tested = Scenes.getScene();
//	size = Tested.size();
//	BOOST_REQUIRE_EQUAL(size, 1);
//	active = Tested[0]->getIsVisible();
//	BOOST_CHECK_EQUAL(active, 1);
//	ID = Tested[0]->getID();
//	BOOST_CHECK_EQUAL(ID, 0);
//}
//
//BOOST_AUTO_TEST_CASE(TestPaused)
//{
//	SceneManager Scenes;
//	BOOST_CHECK_EQUAL(Scenes.init(nullptr, nullptr, nullptr, nullptr),true);
//	std::vector<IScene::ptr> Tested;
//
//	//Simulate keystroke.
//	Scenes.keyStroke("J", 0.f);
//	float dt = 0.f;
//	Scenes.onFrame(dt);
//	Tested = Scenes.getScene();
//
//	//Change list to run and confirm.
//	int size = Tested.size();
//	BOOST_REQUIRE_EQUAL(size, 2);
//	int active = Tested[0]->getIsVisible();
//	BOOST_CHECK_EQUAL(active, 1);
//	int ID = Tested[0]->getID();
//	BOOST_CHECK_EQUAL(ID, 0);
//	active = Tested[1]->getIsVisible();
//	BOOST_CHECK_EQUAL(active, 1);
//	ID = Tested[1]->getID();
//	BOOST_CHECK_EQUAL(ID, 1);
//	
//	Scenes.keyStroke("K", 0.f);
//	Tested = Scenes.getScene();
//	size = Tested.size();
//	
//	//Confirms that a pause scene is shown above the others.	
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
//	//Removes the pause scene.
//	Scenes.keyStroke("K", 0.f);
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
//	BOOST_CHECK_EQUAL(ID,1);
//}
//
//
//BOOST_AUTO_TEST_SUITE_END()