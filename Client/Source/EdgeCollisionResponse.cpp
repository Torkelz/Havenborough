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

void EdgeCollisionResponse::handleCollision(Player *p_Player, unsigned int p_CollisionBody)
{
	XMFLOAT3 playerStartPos = p_Player->getPosition();
	XMFLOAT4 BC = Vector4ToXMFLOAT4(&m_Physics->getBodyPosition(p_CollisionBody));
	XMFLOAT3 boundingVolumeCenter = XMFLOAT3(BC.x,BC.y,BC.z);
	//Step 1 vector from player to center of boundingvolume
	XMFLOAT3 pToBC;
	XMStoreFloat3(&pToBC,XMLoadFloat3(&boundingVolumeCenter) - XMLoadFloat3(&playerStartPos));
	// reflect
	//XMVector3Reflect(XMLoadFloat3(&pToBC), )

	XMFLOAT3 playerEndPos;
}