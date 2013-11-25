#pragma once

#include "IPhysics.h"

class Physics : public IPhysics
{
private:
	Collision m_collision;
public:
	Physics(void){};
	~Physics(void){};

	void initialize();
		 
	bool sphereVsSphere( Sphere* p_sphere1, Sphere* p_sphere2 );
	bool AABBvsAABB( AABB* p_aabb1, AABB* p_aabb2 );
	bool AABBvsSphere( AABB* p_aabb, Sphere* p_sphere);
};

