#include "EdgeCollisionResponse.h"

EdgeCollisionResponse::EdgeCollisionResponse(void)
{
	m_Physics = nullptr;
}


EdgeCollisionResponse::~EdgeCollisionResponse(void)
{
	m_Physics = nullptr;
}

void EdgeCollisionResponse::initialize( IPhysics *p_Physics )
{
	m_Physics = p_Physics;
}

void EdgeCollisionResponse::handleCollision(Player *p_Player, unsigned int p_CollisionBody, XMVECTOR p_VictimNormal)
{
	XMVECTOR playerStartPos = XMLoadFloat3(&p_Player->getPosition());
	XMFLOAT4 BC = Vector4ToXMFLOAT4(&m_Physics->getBodyPosition(p_CollisionBody));

	XMVECTOR boundingVolumeCenter = XMLoadFloat3(&XMFLOAT3(BC.x,BC.y,BC.z));
	//Step 1 vector from player to center of boundingvolume
	//XMFLOAT3 pToBC;
	//XMStoreFloat3(&pToBC,XMLoadFloat3(&boundingVolumeCenter) - XMLoadFloat3(&playerStartPos));
	// reflect
	//XMVector3Reflect(XMLoadFloat3(&pToBC), )
	p_VictimNormal = XMVector3Normalize(p_VictimNormal);
	XMVECTOR playerEndPos =  calculatePosition(p_VictimNormal, boundingVolumeCenter - playerStartPos,
		boundingVolumeCenter );

	p_Player->forceMove(playerStartPos, playerEndPos);
}

XMVECTOR EdgeCollisionResponse::calculatePosition(XMVECTOR p_Normal, XMVECTOR p_PlayerToCenter,
	XMVECTOR p_BodyCenter)
{
	//VERSION 1//
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

	//VERSION 2//
	//XMVECTOR point7 = XMLoadFloat3(&XMFLOAT3(10,5,5));
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0,1,0));
	//XMVECTOR dirr = XMVector3Normalize(point3 - point7);
	//XMVECTOR cross = XMVector3Cross(dirr, up);

	XMVECTOR playerToCenterLength = XMVector3Length(p_PlayerToCenter);
	p_PlayerToCenter = XMVector3Normalize(p_PlayerToCenter);
	p_PlayerToCenter = XMVector3Reflect(p_PlayerToCenter, p_Normal);
	p_PlayerToCenter = XMVector3Reflect(-p_PlayerToCenter, up);

	XMVECTOR playerFinal = p_BodyCenter + (playerToCenterLength) * p_PlayerToCenter;

	return playerFinal;
}
