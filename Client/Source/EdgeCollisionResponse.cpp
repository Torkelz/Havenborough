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

void EdgeCollisionResponse::handleCollision(Player *p_Player, unsigned int p_CollisionBody, XMVECTOR p_VictimNormal)
{
	XMVECTOR playerStartPos = XMLoadFloat3(&p_Player->getPosition());
	XMFLOAT4 collisionBodyPos = Vector4ToXMFLOAT4(&m_Physics->getBodyPosition(p_CollisionBody));

	XMVECTOR boundingVolumeCenter = XMLoadFloat3(&XMFLOAT3(collisionBodyPos.x, collisionBodyPos.y, collisionBodyPos.z));

	p_VictimNormal = XMVector3Normalize(p_VictimNormal);
	XMVECTOR playerEndPos =  calculateEndPosition(p_VictimNormal, boundingVolumeCenter - playerStartPos,
		boundingVolumeCenter );

	p_Player->forceMove(playerStartPos, playerEndPos);
}

XMVECTOR EdgeCollisionResponse::calculateEndPosition(XMVECTOR p_Normal, XMVECTOR p_PlayerToCenter,
	XMVECTOR p_BodyCenter)
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
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0,1,0));

	XMVECTOR playerToCenterLength = XMVector3Length(p_PlayerToCenter);
	p_PlayerToCenter = XMVector3Normalize(p_PlayerToCenter);
	p_PlayerToCenter = XMVector3Reflect(p_PlayerToCenter, p_Normal);
	p_PlayerToCenter = XMVector3Reflect(-p_PlayerToCenter, up);

	XMVECTOR playerFinal = p_BodyCenter + (playerToCenterLength) * p_PlayerToCenter;


	//TODO: Fix offset of height
	return playerFinal;
}