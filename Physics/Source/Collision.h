#pragma once

#include "VolumeIncludeAll.h"

class Collision
{
public:
	static enum { OUTSIDE = 0, INTERSECT, INSIDE };
public:
	Collision();
	~Collision();
	
	/**
	* Sphere versus Sphere collision
	* @return true if collision happens, otherwise false.
	*/
	bool	sphereVsSphere(Sphere* p_sphere1, Sphere* p_sphere2);
	/**
	* AABB versus AABB collision
	* ## Tip: You can use the AABBs' spheres and do SphereVsSphere before calling this 
	* if you are doing a lot of collision tests and expect very few of them to return 
	* true for better performance. ##
	* @return true if collision happens, otherwise false
	*/
	bool	AABBvsAABB( AABB* p_aabb1, AABB* p_aabb2 );
	/**
	* AABB versus Sphere collision
	* ## Tip: You can use the AABB's sphere and do SphereVsSphere before calling this 
	* if you are doing a lot of collision tests and expect very few of them to return 
	* true for better performance. ##
	* @return true if collision happens, otherwise false
	*/
	bool	AABBvsSphere( AABB* p_aabb, Sphere* p_sphere );
	//bool	collide( BoundingVolume* p_pVolume );
};

