#include <boost/test/unit_test.hpp>
#include "../../Client/Source/Player.h"

#include <ActorFactory.h>
#include <TweakSettings.h>
#include <Utilities/MemoryUtil.h>

BOOST_AUTO_TEST_SUITE(PlayerTest)

using namespace DirectX;

/**
 * Player set and get Tests
 */
BOOST_AUTO_TEST_CASE(GetSet)
{
	TweakSettings::initializeMaster();

	Player player;
	ActorFactory factory(0);
	static const char* playerDesc =
		"<Object>"
		"	<RunControl />"
		"</Object>";
	tinyxml2::XMLDocument doc;
	doc.Parse(playerDesc);

	Actor::ptr runActor(factory.createActor(doc.FirstChildElement("Object")));
	player.setActor(runActor);

	BOOST_CHECK(player.getForceMove() == false);
	BOOST_CHECK(player.getDirection() == Vector3(0,0,0));
	player.setDirection(Vector3(1.f, 0.f, 2.f));
	BOOST_CHECK(player.getDirection() == Vector3(1,0,2));
	player.setAllowedToMove(true);
	BOOST_CHECK(player.getAllowedToMove() == true);

	TweakSettings::shutdown();
}

BOOST_AUTO_TEST_CASE(GetSetHeight)
{
	Player player;
	float heightForTest = 170.0f;
	player.setHeight(heightForTest);
	BOOST_CHECK(player.getHeight() == heightForTest);
	BOOST_CHECK(player.getChestHeight() == heightForTest * 0.75f);
	BOOST_CHECK(player.getWaistHeight() == heightForTest * 0.5f);
	BOOST_CHECK(player.getKneeHeight() == heightForTest * 0.25f);
}

BOOST_AUTO_TEST_CASE(GetSetgroundNormal)
{
	TweakSettings::initializeMaster();

	Player player;
	ActorFactory factory(0);
	static const char* playerDesc =
		"<Object>"
		"	<RunControl />"
		"</Object>";
	tinyxml2::XMLDocument doc;
	doc.Parse(playerDesc);

	Actor::ptr runActor(factory.createActor(doc.FirstChildElement("Object")));
	player.setActor(runActor);

	BOOST_CHECK_EQUAL(player.getGroundNormal().x, 0);
	BOOST_CHECK_EQUAL(player.getGroundNormal().y, 1);
	BOOST_CHECK_EQUAL(player.getGroundNormal().z, 0);
	player.setGroundNormal(XMFLOAT3(1,0,0));
	DirectX::XMFLOAT3 norm = player.getGroundNormal();
	BOOST_CHECK_EQUAL(norm.x, 1);
	BOOST_CHECK_EQUAL(norm.y, 0);
	BOOST_CHECK_EQUAL(norm.z, 0);

	TweakSettings::shutdown();
}

BOOST_AUTO_TEST_CASE(GetSetMana)
{
	Player player;
	float maxMana = player.getMaxMana();

	//BOOST_CHECK(player.getCurrentMana() == maxMana);

	player.setCurrentMana(0.f);
	BOOST_CHECK_EQUAL(player.getCurrentMana(), 0.f);
	//BOOST_CHECK_EQUAL(player.getPreviousMana(), maxMana);

	player.setCurrentMana(-10.f);
	BOOST_CHECK_EQUAL(player.getCurrentMana(), 0.f);
	BOOST_CHECK_EQUAL(player.getPreviousMana(), 0.f);

	player.setCurrentMana(maxMana + 10.f);
	BOOST_CHECK_EQUAL(player.getCurrentMana(), maxMana);
	BOOST_CHECK_EQUAL(player.getPreviousMana(), 0.f);
}

BOOST_AUTO_TEST_SUITE_END()