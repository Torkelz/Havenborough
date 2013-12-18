#pragma once
#include "IPhysics.h"
#include "Player.h"

class EdgeCollisionResponse
{
private:
	IPhysics *m_Physics;

public:
	/**
	* Constructor
	*/
	EdgeCollisionResponse(void);
	
	/**
	* Desctructor
	*/
	~EdgeCollisionResponse(void);
	
	/**
	* Initialize the edge collision response to communicate with the physics engine.
	* @param p_Physics pointer to the physics engine
	*/
	void initialize(IPhysics *p_Physics);
	
	/**
	* Checks if a player has collided with an edge. If collision has occurred the player will be moved to a
	* calculated position.
	* @param p_Hit information about the collision hit
	* @param p_Player the player to be evaluated with the collision
	* @return true = player collided with an edge and will be moved, false = player did not collide and has full control
	*/
	bool checkCollision(HitData &p_Hit, Player *p_Player);

private:
	void handleCollision(Player *p_Player, unsigned int p_CollisionBody, XMVECTOR p_VictimNormal, 
		XMVECTOR p_CollisionPosition);

	XMVECTOR calculateEndPosition(XMVECTOR p_Normal, XMVECTOR p_PlayerToCenter,
		XMVECTOR p_BodyCenter, XMVECTOR *p_CollisionPosition, float p_HeightOffset);
};