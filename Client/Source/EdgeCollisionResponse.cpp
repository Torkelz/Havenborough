#include "EdgeCollisionResponse.h"
#include "ClientExceptions.h"

EdgeCollisionResponse::EdgeCollisionResponse(void)
{
	m_Physics = nullptr;
}

EdgeCollisionResponse::~EdgeCollisionResponse(void)
{
	m_Physics = nullptr;
}

void EdgeCollisionResponse::initialize(IPhysics *p_Physics)
{
	if(!p_Physics)
		throw EdgeCollisionException("Error when initializing the edge collision response, physics engine not initialized ",
		__LINE__, __FILE__);

	m_Physics = p_Physics;
}

bool EdgeCollisionResponse::checkCollision(HitData &p_Hit, Player *p_Player)
{
	if(p_Hit.isEdge && p_Hit.collider == p_Player->getBody())
	{
		XMFLOAT3 collisionNormal = Vector3ToXMFLOAT3(&p_Hit.colNorm);
		XMFLOAT3 collisionPosition = Vector3ToXMFLOAT3(&p_Hit.colPos);
		handleCollision(p_Player, p_Hit.collisionVictim, XMLoadFloat3(&collisionNormal),
			XMLoadFloat3(&collisionPosition));
		return true;
	}
	return false;
}

void EdgeCollisionResponse::handleCollision(Player *p_Player, unsigned int p_CollisionBody, XMVECTOR p_VictimNormal,
	XMVECTOR p_CollisionPosition)
{
	XMFLOAT3 playerPos = p_Player->getPosition();
	XMVECTOR playerStartPos = XMLoadFloat3(&playerPos);
	Vector4 bodyPositionV = m_Physics->getBodyPosition(p_CollisionBody);
	XMFLOAT4 collisionBodyPos = Vector4ToXMFLOAT4(&bodyPositionV);

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

XMVECTOR EdgeCollisionResponse::calculateEndPosition(XMVECTOR p_Normal, XMVECTOR p_PlayerToCenter,
	XMVECTOR p_BodyCenter, XMVECTOR *p_CollisionPosition, float p_HeightOffset)
{
#pragma region VERSION 1
	/*XMVECTOR point6 = XMLoadFloat3(&XMFLOAT3(-10,-5,-5));
	XMVECTOR point3 = XMLoadFloat3(&XMFLOAT3(-10,5,5));
	XMVECTOR point4 = XMLoadFloat3(&XMFLOAT3(-10,-5,5));
	XMVECTOR center = XMLoadFloat3(&XMFLOAT3(0,0,0));
	XMVECTOR player = XMLoadFloat3(&XMFLOAT3(8,-7,-7));
	XMVECTOR direction = XMVector3Normalize(XMVector3Cross(point6 - point4, point3 - point4));
	//XMVECTOR t = direction;
	XMVECTOR b = center - player;
	XMFLOAT3 xDir, xB, xT;
	XMStoreFloat3(&xDir, direction);
	XMStoreFloat3(&xT, direction);
	XMStoreFloat3(&xB, b);

	float tx = xDir.x * xT.x;
	float ty = xDir.y * xT.y;
	float tz = xDir.z * xT.z;
	float bx = xDir.x * xB.x;
	float by = xDir.y * xB.y;
	float bz = xDir.z * xB.z;
	float T = tx+ty+tz;
	float B = bx+by+bz;
	B *= -1.0f;

	float end = B/T;
	XMVECTOR pointOnLine = center + end*direction;

	XMVECTOR playerDir = pointOnLine - player;

	XMVECTOR playerEnd = player + playerDir*2;//*/
#pragma endregion

	//VERSION 2//
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