#include <boost/test/unit_test.hpp>
#include "../../Client/Source/EdgeCollisionResponse.h"
#include "../../Client/Source/Player.h"
#include <Utilities/MemoryUtil.h>

BOOST_AUTO_TEST_SUITE(EdgeCollisionResponseTest)

using namespace DirectX;

class DummyPlayer : public Player
{
public:
	XMFLOAT3 m_Position;
	float m_Height;
	BodyHandle m_Body;
	bool m_ForceMove;
	
	DummyPlayer()
	{
		m_ForceMove = false;
	}

	~DummyPlayer()
	{

	}

	BodyHandle getBody(int p_Index) const override
	{
		return m_Body;
	}

	XMFLOAT3 getPosition() const override
	{
		return m_Position;
	}

	bool getForceMove() override
	{
		return m_ForceMove;
	}

	void setForceMove(bool p)
	{
		m_ForceMove = p;
	}

	void forceMove(std::string, XMFLOAT3, XMFLOAT3, XMFLOAT3) override
	{
		if(!m_ForceMove)
			m_ForceMove = true;
	}
};

BOOST_AUTO_TEST_CASE(CheckCollisionWrongID)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	EdgeCollisionResponse edgeCollision;
	
	player->m_Body = 2;

	data.collisionVictim = 1;
	data.collider = 3;
	data.isEdge = true;

	Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 edgeOrientation = Vector3(1.0f, 1.0f, 0.0f);
	bool ret = edgeCollision.checkCollision(data, pos, edgeOrientation, player);
	BOOST_CHECK(ret == false);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDBelowPos)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	EdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 2.0f, 0.0f);
	player->setHeight(4.f);

	Vector3 pos = Vector3(0.0f, 4.0f, 0.0f);
	
	data.colNorm = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	data.colPos = Vector4(0.5f, 3.5f, 0.0f, 0.0f);
	data.collisionVictim = 1;
	data.collider = 2;
	data.isEdge = true;
	
	Vector3 edgeOrientation = Vector3(1.0f, 1.0f, 0.0f);

	edgeCollision.checkCollision(data, pos, edgeOrientation, player);
	
	BOOST_CHECK(player->getForceMove() == true);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDKneeHeightPos)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	EdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 5.0f, 0.0f);
	player->setHeight(4.f);

	Vector3 pos = Vector3(0.0f, 4.0f, 0.0f);

	data.colNorm = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	data.colPos = Vector4(0.5f, 3.5f, 0.0f, 0.0f);
	data.collisionVictim = 1;
	data.collider = 2;
	data.isEdge = true;
	Vector3 edgeOrientation = Vector3(1.0f, 1.0f, 0.0f);
	edgeCollision.checkCollision(data, pos, edgeOrientation, player);

	BOOST_CHECK(player->getForceMove() == false);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDAbovePos)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	EdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 8.0f, 0.0f);
	player->setHeight(4.f);

	Vector3 pos = Vector3(0.0f, 4.0f, 0.0f);

	data.colNorm = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	data.colPos = Vector4(0.5f, 3.5f, 0.0f, 0.0f);
	data.collisionVictim = 1;
	data.collider = 2;
	data.isEdge = true;

	Vector3 edgeOrientation = Vector3(1.0f, 1.0f, 0.0f);
	edgeCollision.checkCollision(data, pos, edgeOrientation, player);

	BOOST_CHECK(player->getForceMove() == false);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDKneeHeight)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	EdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 4.0f, 0.0f);
	player->setHeight(4.f);

	Vector3 pos = Vector3(0.0f, 4.0f, 0.0f);

	data.colNorm = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	data.colPos = Vector4(0.5f, 3.5f, 0.0f, 0.0f);
	data.collisionVictim = 1;
	data.collider = 2;
	data.isEdge = true;

	Vector3 edgeOrientation = Vector3(1.0f, 1.0f, 0.0f);
	edgeCollision.checkCollision(data, pos, edgeOrientation, player);

	BOOST_CHECK(player->getForceMove() == true);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDChestHeight)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	EdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	player->setHeight(4.f);

	Vector3 pos = Vector3(0.0f, 4.0f, 0.0f);

	data.colNorm = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	data.colPos = Vector4(0.5f, 3.5f, 0.0f, 0.0f);
	data.collisionVictim = 1;
	data.collider = 2;
	data.isEdge = true;

	Vector3 edgeOrientation = Vector3(1.0f, 1.0f, 0.0f);
	edgeCollision.checkCollision(data, pos, edgeOrientation, player);

	BOOST_CHECK(player->getForceMove() == true);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDHeight)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	EdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	player->setHeight(4.f);

	Vector3 pos = Vector3(0.0f, 4.0f, 0.0f);

	data.colNorm = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	data.colPos = Vector4(0.5f, 3.5f, 0.0f, 0.0f);
	data.collisionVictim = 1;
	data.collider = 2;
	data.isEdge = true;

	Vector3 edgeOrientation = Vector3(1.0f, 1.0f, 0.0f);
	edgeCollision.checkCollision(data, pos, edgeOrientation, player);

	BOOST_CHECK(player->getForceMove() == true);

	SAFE_DELETE(player);
}
BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDReturn)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	EdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 4.8f, 0.0f);
	player->setHeight(4.f);

	Vector3 pos = Vector3(0.0f, 4.0f, 0.0f);

	data.colNorm = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	data.colPos = Vector4(0.5f, 3.5f, 0.0f, 0.0f);
	data.collisionVictim = 1;
	data.collider = 2;
	data.isEdge = true;

	Vector3 edgeOrientation = Vector3(1.0f, 1.0f, 0.0f);
	edgeCollision.checkCollision(data, pos, edgeOrientation, player);

	BOOST_CHECK(player->getForceMove() == false);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_SUITE_END()