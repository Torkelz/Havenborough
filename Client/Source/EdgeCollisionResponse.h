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
	* @param p_EdgeSizeY half the height of the collision volume
	* @param p_Player the player to be evaluated with the collision
	* @return true = player collided with an edge and will be moved, false = player did not collide and has full control
	*/
	bool checkCollision(HitData &p_Hit, Vector3 p_EdgePosition, float p_EdgeSizeY, Player *p_Player);

protected:
	void handleCollision(Player *p_Player, Vector3 p_EdgePosition, DirectX::XMVECTOR p_VictimNormal, 
		float p_EdgeOffsetY);

	DirectX::XMVECTOR calculateEndPosition(DirectX::XMVECTOR p_Normal, DirectX::XMVECTOR p_PlayerToCenter,
		DirectX::XMVECTOR p_BodyCenter, float p_EdgeOffsetY, float p_HeightOffset);
};