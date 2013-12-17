#pragma once
#include "IPhysics.h"
#include "Player.h"

class EdgeCollisionResponse
{
private:
	IPhysics *m_Physics;

public:
	EdgeCollisionResponse(void);
	~EdgeCollisionResponse(void);
	void initialize(IPhysics *p_Physics);

	void handleCollision(Player *p_Player, unsigned int p_CollisionBody, XMVECTOR p_VictimNormal);
	XMVECTOR calculatePosition(XMVECTOR p_Normal, XMVECTOR p_PlayerToCenter,
		XMVECTOR p_BodyCenter);

};

