#include "EdgeCollisionResponse.h"
#include "ClientExceptions.h"

using namespace DirectX;

EdgeCollisionResponse::EdgeCollisionResponse(void)
{
}

EdgeCollisionResponse::~EdgeCollisionResponse(void)
{
}

bool EdgeCollisionResponse::checkCollision(HitData &p_Hit, Vector3 p_EdgePosition, Vector3 p_EdgeOrientation, Player *p_Player)
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

void EdgeCollisionResponse::handleCollision(Player *p_Player, Vector3 p_EdgePosition, XMVECTOR p_VictimNormal,
	Vector3 p_EdgeOrientation)
{
	XMFLOAT3 playerOrigPos = p_Player->getPosition();

	if(playerOrigPos.y < p_EdgePosition.y)
	{
		DirectX::XMFLOAT3 victimNormal;
		DirectX::XMStoreFloat3(&victimNormal, p_VictimNormal);

		if (playerOrigPos.y + 10 > p_EdgePosition.y)
			return;
		else if(playerOrigPos.y  + p_Player->getKneeHeight() - 20.0f > p_EdgePosition.y)
			p_Player->forceMove("Climb1", victimNormal, p_EdgePosition, p_EdgeOrientation);
		else if(playerOrigPos.y  + p_Player->getWaistHeight() > p_EdgePosition.y)
			p_Player->forceMove("Climb2", victimNormal, p_EdgePosition, p_EdgeOrientation);
		else if(playerOrigPos.y  + p_Player->getChestHeight() > p_EdgePosition.y)
			p_Player->forceMove("Climb3", victimNormal, p_EdgePosition, p_EdgeOrientation);
		else if(playerOrigPos.y  + p_Player->getHeight() > p_EdgePosition.y)
			p_Player->forceMove("Climb4", victimNormal, p_EdgePosition, p_EdgeOrientation);
		else{}
	}
}