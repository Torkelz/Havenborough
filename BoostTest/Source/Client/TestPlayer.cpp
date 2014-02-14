#include <boost/test/unit_test.hpp>
#include "../../Client/Source/Player.h"
#include <Utilities/MemoryUtil.h>

BOOST_AUTO_TEST_SUITE(PlayerTest)

using namespace DirectX;

/**
 * Player set and get Tests
 */
BOOST_AUTO_TEST_CASE(GetSet)
{
	Player player;
	BOOST_CHECK(player.getForceMove() == false);
	BOOST_CHECK(player.getDirection() == Vector3(0,0,0));
	player.setDirectionX(1);
	player.setDirectionZ(2);
	BOOST_CHECK(player.getDirection() == Vector3(1,0,2));
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
	Player player;
	BOOST_CHECK(player.getGroundNormal().x == 0);
	BOOST_CHECK(player.getGroundNormal().y == 1);
	BOOST_CHECK(player.getGroundNormal().z == 0);
	player.setGroundNormal(XMFLOAT3(1,0,0));
	BOOST_CHECK(player.getGroundNormal().x == 1);
	BOOST_CHECK(player.getGroundNormal().y == 0);
	BOOST_CHECK(player.getGroundNormal().z == 0);
}

BOOST_AUTO_TEST_SUITE_END()