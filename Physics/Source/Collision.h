#pragma once

#include "VolumeIncludeAll.h"
#include "..\include\PhysicsTypes.h"

class Collision
{
public:
	Collision();
	~Collision();

	/**
	* Redirect to the appropriate check, when neither BoundingVolumes' type is known.
	* @return HitData, see HitData definition.
	*/
	HitData boundingVolumeVsBoundingVolume(BoundingVolume* p_Volume1, BoundingVolume* p_Volume2);
	/**
	* Check for the appropriate collision, a BoundingVolume versus a sphere.
	* @return HitData, see HitData definition.
	*/
	HitData boundingVolumeVsSphere(BoundingVolume* p_Volume, Sphere* p_Sphere);
	/**
	* Check for the appropriate collision, a BoundingVolume versus an AABB.
	* Sphere versus Sphere collision
	* @return HitData, see HitData definition.
	*/
	HitData boundingVolumeVsAABB(BoundingVolume* p_Volume, AABB* p_AABB);
	/**
	* Sphere versus Sphere collision
	* @return HitData, see HitData definition.
	*/
	HitData sphereVsSphere(Sphere* p_Sphere1, Sphere* p_Sphere2);
	/**
	* AABB versus AABB collision
	* ## SphereVsSphere check happens before 
	* the actual AABBvsAABB collision check. ##
	* @return HitData, see HitData definition.
	*/
	HitData AABBvsAABB( AABB* p_AABB1, AABB* p_AABB2 );
	/**
	* AABB versus Sphere collision
	* ## SphereVsSphere check happens before 
	* the actual AABBvsAABB collision check. ##
	* @return HitData, see HitData definition.
	*/
	HitData AABBvsSphere( AABB* p_AABB, Sphere* p_Sphere );
	//bool	collide( BoundingVolume* p_pVolume );
};

