#pragma once
#include "Player.h"

#include <DirectXMath.h>

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
	* @param p_EdgePosition center position of collision volume
	* @param p_EdgeExtents "radius" of the edge
	* @param p_Player the player to be evaluated with the collision
	* @return true = player collided with an edge and will be moved, false = player did not collide and has full control
	*/
	bool checkCollision(HitData &p_Hit, Vector3 p_EdgePosition, Vector3 p_EdgeExtents, Player *p_Player);

protected:
	void handleCollision(Player *p_Player, Vector3 p_EdgePosition, DirectX::XMVECTOR p_VictimNormal, 
		Vector3 p_EdgeOrientation);

	Vector3 calculateEdgeOrientation(Vector3 p_EdgeExtents);
};