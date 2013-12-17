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
	* Handles the collision of a player and the edge of a mountable object. Updates the players final position.
	* @param p_Player the player which should be handled
	* @param p_CollisionBody the ID of the edge's body handle
	* @param p_VictimNormal the edge's normal in the collision
	*/
	void handleCollision(Player *p_Player, unsigned int p_CollisionBody, XMVECTOR p_VictimNormal);
	
private:
	XMVECTOR calculateEndPosition(XMVECTOR p_Normal, XMVECTOR p_PlayerToCenter,
		XMVECTOR p_BodyCenter);
};