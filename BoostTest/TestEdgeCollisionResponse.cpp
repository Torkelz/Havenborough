#include <boost/test/unit_test.hpp>
#include "../../Client/Source/EdgeCollisionResponse.h"
#include "../../Client/Source/Player.h"

BOOST_AUTO_TEST_SUITE(EdgeCollisionResponseTest)

class DummyPlayer : public Player
{
public:
	BodyHandle body;
	
	DummyPlayer()
	{
		body = 2;
	}

	~DummyPlayer()
	{

	}
};

BOOST_AUTO_TEST_CASE(TestLogging)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	EdgeCollisionResponse edgeCollision;

	data.collisionVictim = 1;
	data.collider = 3;
	data.isEdge = true;
	Vector4 pos = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	BOOST_CHECK(edgeCollision.checkCollision(data, pos, player) == false);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_SUITE_END()