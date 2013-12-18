#pragma once
#include "IPhysics.h"
#include "Player.h"

class EdgeCollisionResponse
{
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
	* Checks if a player has collided with an edge. If collision has occurred the player will be moved to a
	* calculated position.
	* @param p_Hit information about the collision hit
	* @param p_EdgePosition center positn
	* @param p_Player the player to be evaluated with the collision
	* @return true = player collided with an edge and will be moved, false = player did not collide and has full control
	*/
	bool checkCollision(HitData &p_Hit, Vector4 p_EdgePosition, Player *p_Player);

private:
	void handleCollision(Player *p_Player, Vector4 p_EdgePosition, XMVECTOR p_VictimNormal, 
		XMVECTOR p_CollisionPosition);

	XMVECTOR calculateEndPosition(XMVECTOR p_Normal, XMVECTOR p_PlayerToCenter,
		XMVECTOR p_BodyCenter, XMVECTOR *p_CollisionPosition, float p_HeightOffset);
};