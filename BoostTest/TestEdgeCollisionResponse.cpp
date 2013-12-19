#include <boost/test/unit_test.hpp>
#include "../../Client/Source/EdgeCollisionResponse.h"
#include "../../Client/Source/Player.h"

BOOST_AUTO_TEST_SUITE(EdgeCollisionResponseTest)

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

	float getHeight()
	{
		return m_Heigt;
	}

	bool getForceMove()
	{
		return m_ForceMove;
	}

	void forceMove(XMVECTOR, XMVECTOR)
	{
		if(!m_ForceMove)
			m_ForceMove = true;
	}
};

class DummeEdgeCollisionResponse
{
public:
	DummeEdgeCollisionResponse()
	{
	}

	~DummeEdgeCollisionResponse()
	{
	}

	bool checkCollision(HitData &p_Hit, Vector4 p_EdgePosition, DummyPlayer *p_Player)
	{
		if(p_Hit.isEdge && p_Hit.collider == p_Player->getBody())
		{
			XMFLOAT3 collisionNormal = Vector4ToXMFLOAT3(&p_Hit.colNorm);
			XMFLOAT3 collisionPosition = Vector4ToXMFLOAT3(&p_Hit.colPos);
			handleCollision(p_Player, p_EdgePosition, XMLoadFloat3(&collisionNormal),
				XMLoadFloat3(&collisionPosition));
			return true;
		}
		return false;
	}

protected:
	void handleCollision(DummyPlayer *p_Player, Vector4 p_EdgePosition, XMVECTOR p_VictimNormal,
		XMVECTOR p_CollisionPosition)
	{

		XMFLOAT3 playerPos = p_Player->getPosition();
		XMVECTOR playerStartPos = XMLoadFloat3(&playerPos);
		XMFLOAT4 collisionBodyPos = Vector4ToXMFLOAT4(&p_EdgePosition);

		XMFLOAT3 collisionPosition = XMFLOAT3(collisionBodyPos.x, collisionBodyPos.y, collisionBodyPos.z);
		XMVECTOR boundingVolumeCenter = XMLoadFloat3(&collisionPosition);

		float playerPositionY = playerPos.y;
		float playerHeight = p_Player->getHeight();
		float kneeHeight = playerHeight * 0.25f; //Assumes knees at roughly 25% of player height

		if(playerPositionY - kneeHeight < collisionBodyPos.y)
		{
			p_VictimNormal = XMVector3Normalize(p_VictimNormal);

			XMVECTOR playerEndPos =  calculateEndPosition(p_VictimNormal, boundingVolumeCenter - playerStartPos,
				boundingVolumeCenter, &p_CollisionPosition, playerHeight* 0.5f);

			p_Player->forceMove(playerStartPos, playerEndPos);
		}
	}
	
	XMVECTOR calculateEndPosition(XMVECTOR p_Normal, XMVECTOR p_PlayerToCenter,
		XMVECTOR p_BodyCenter, XMVECTOR *p_CollisionPosition, float p_HeightOffset)
	{

		XMFLOAT3 up = XMFLOAT3(0,1,0);
		XMVECTOR upV = XMLoadFloat3(&up);

		XMFLOAT3 edgeOffset;
		XMStoreFloat3(&edgeOffset, XMVector3Dot((*p_CollisionPosition - p_BodyCenter), p_Normal));
		float edgeOffsetY = edgeOffset.y;

		XMVECTOR playerToCenterLength = XMVector3Length(p_PlayerToCenter);
		p_PlayerToCenter = XMVector3Normalize(p_PlayerToCenter);
		p_PlayerToCenter = XMVector3Reflect(p_PlayerToCenter, p_Normal);
		p_PlayerToCenter = XMVector3Reflect(-p_PlayerToCenter, upV);

		XMVECTOR playerFinal = p_BodyCenter + (playerToCenterLength) * p_PlayerToCenter;
		XMFLOAT3 offset = XMFLOAT3(0.0f, edgeOffsetY + p_HeightOffset, 0.0f);
		XMVECTOR offsetV = XMLoadFloat3(&offset);
	
		return playerFinal + offsetV;
	}
};


BOOST_AUTO_TEST_CASE(CheckCollisionWrongID)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	DummeEdgeCollisionResponse edgeCollision;
	
	player->m_Body = 2;

	data.collisionVictim = 1;
	data.collider = 3;
	data.isEdge = true;

	Vector4 pos = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	BOOST_CHECK(edgeCollision.checkCollision(data, pos, player) == false);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDBelowPos)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	DummeEdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 2.0f, 0.0f);
	player->m_Heigt = 4.0f;

	Vector4 pos = Vector4(0.0f, 4.0f, 0.0f, 0.0f);
	
	data.colNorm = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	data.colPos = Vector4(0.5f, 3.5f, 0.0f, 0.0f);
	data.collisionVictim = 1;
	data.collider = 2;
	data.isEdge = true;
	
	edgeCollision.checkCollision(data, pos, player);
	
	BOOST_CHECK(player->getForceMove() == true);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDKneeHeightPos)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	DummeEdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 5.0f, 0.0f);
	player->m_Heigt = 4.0f;

	Vector4 pos = Vector4(0.0f, 4.0f, 0.0f, 0.0f);

	data.colNorm = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	data.colPos = Vector4(0.5f, 3.5f, 0.0f, 0.0f);
	data.collisionVictim = 1;
	data.collider = 2;
	data.isEdge = true;

	edgeCollision.checkCollision(data, pos, player);

	BOOST_CHECK(player->getForceMove() == false);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_CASE(CheckCollisionCorrectIDAbovePos)
{
	DummyPlayer *player = new DummyPlayer();
	HitData data;
	DummeEdgeCollisionResponse edgeCollision;

	player->m_Body = 2;
	player->m_Position = XMFLOAT3(1.0f, 8.0f, 0.0f);
	player->m_Heigt = 4.0f;

	Vector4 pos = Vector4(0.0f, 4.0f, 0.0f, 0.0f);

	data.colNorm = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	data.colPos = Vector4(0.5f, 3.5f, 0.0f, 0.0f);
	data.collisionVictim = 1;
	data.collider = 2;
	data.isEdge = true;

	edgeCollision.checkCollision(data, pos, player);

	BOOST_CHECK(player->getForceMove() == false);

	SAFE_DELETE(player);
}

BOOST_AUTO_TEST_SUITE_END()