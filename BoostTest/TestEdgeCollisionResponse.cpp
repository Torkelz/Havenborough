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
	float m_Heigt;
	BodyHandle m_Body;
	bool m_ForceMove;
	
	DummyPlayer()
	{
		m_ForceMove = false;
	}

	~DummyPlayer()
	{

	}

	BodyHandle getBody()
	{
		return m_Body;
	}

	XMFLOAT3 getPosition()
	{
		return m_Position;
	}

	bool getForceMove()
	{
		return m_ForceMove;
	}

	void setForceMove(bool p)
	{
		m_ForceMove = p;
	}

	void forceMove(std::string, XMFLOAT3, XMFLOAT3, XMFLOAT3)
	{
		if(!m_ForceMove)
			m_ForceMove = true;
	}

	float getHeight() const
	{
		return m_Heigt;
	}

	float getChestHeight() const
	{
		return m_Heigt * 0.75f;
	}

	float getWaistHeight() const
	{
		return m_Heigt * 0.5f;
	}

	float getKneeHeight() const
	{
		return m_Heigt * 0.25f;
	}
};

class DummyEdgeCollisionResponse
{
public:
	DummyEdgeCollisionResponse()
	{
	}

	~DummyEdgeCollisionResponse()
	{
	}

	bool checkCollision(HitData &p_Hit, Vector3 p_EdgePosition, Vector3 p_EdgeOrientation, DummyPlayer *p_Player)
{
	if(!p_Player->getForceMove() && p_Hit.collider == p_Player->getBody())
	{
		XMFLOAT3 collisionNormal = Vector4ToXMFLOAT3(&p_Hit.colNorm);

		p_Player->setGroundNormal(collisionNormal);

		if (p_Hit.isEdge)
		{
			handleCollision(p_Player, p_EdgePosition, XMLoadFloat3(&collisionNormal),
				p_EdgeOrientation);
		}
		return true;
	}
	return false;
}

protected:
	void DummyEdgeCollisionResponse::handleCollision(DummyPlayer *p_Player, Vector3 p_EdgePosition, XMVECTOR p_VictimNormal,
	Vector3 p_EdgeOrientation)
{

		XMFLOAT3 playerOrigPos = p_Player->getPosition();

		XMVECTOR vReachPointCenter = XMLoadFloat3(&playerOrigPos) - Vector3ToXMVECTOR(&p_EdgePosition, 0);

		XMVECTOR vEdgeOrientation = Vector3ToXMVECTOR(&p_EdgeOrientation, 0);
		vEdgeOrientation = XMVector3Normalize(vEdgeOrientation);

		vReachPointCenter = (XMVector3Dot(vReachPointCenter, vEdgeOrientation) * vEdgeOrientation) + Vector3ToXMVECTOR(&p_EdgePosition, 0);
		XMFLOAT3 nearestPoint;
		XMStoreFloat3(&nearestPoint, vReachPointCenter);


		if(playerOrigPos.y < nearestPoint.y)
		{
			DirectX::XMFLOAT3 victimNormal;
			DirectX::XMStoreFloat3(&victimNormal, p_VictimNormal);

			if (playerOrigPos.y + p_Player->getHeight() * 0.05f > nearestPoint.y)
				p_Player->setForceMove(false);
			else if(playerOrigPos.y  + p_Player->getKneeHeight() - 20.0f > nearestPoint.y)
				p_Player->forceMove("Climb1", victimNormal, p_EdgePosition, p_EdgeOrientation);
			else if(playerOrigPos.y  + p_Player->getWaistHeight() > nearestPoint.y)
				p_Player->forceMove("Climb2", victimNormal, p_EdgePosition, p_EdgeOrientation);
			else if(playerOrigPos.y  + p_Player->getChestHeight() > nearestPoint.y)
				p_Player->forceMove("Climb3", victimNormal, p_EdgePosition, p_EdgeOrientation);
			else if(playerOrigPos.y  + p_Player->getHeight() > nearestPoint.y)
				p_Player->forceMove("Climb4", victimNormal, p_EdgePosition, p_EdgeOrientation);
			else{}
		}
	}
};


BOOST_AUTO_TEST_CASE(CheckCollisionWrongID)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	DummyEdgeCollisionResponse edgeCollision;
	
	player->m_Body = 2;

	data.collisionVictim = 1;
	data.collider = 3;
	data.isEdge = true;

	Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 edgeOrientation = Vector3(1.0f, 1.0f, 0.0f);
	BOOST_CHECK(edgeCollision.checkCollision(data, pos, edgeOrientation, player) == false);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDBelowPos)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	DummyEdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 2.0f, 0.0f);
	player->m_Heigt = 4.0f;

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
	DummyEdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 5.0f, 0.0f);
	player->m_Heigt = 4.0f;

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
	DummyEdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 8.0f, 0.0f);
	player->m_Heigt = 4.0f;

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