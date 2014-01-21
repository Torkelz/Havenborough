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
	* @return HitData, see HitData definition.
	*/
	HitData boundingVolumeVsAABB(BoundingVolume* p_Volume, AABB* p_AABB);
	/**
	* Check for the appropriate collision, a BoundingVolume versus an OBB.
	* @return HitData, see HitData definition.
	*/
	HitData boundingVolumeVsOBB(BoundingVolume* p_Volume, OBB* p_OBB);
	/**
	* Check for the appropriate collision, a BoundingVolume versus a Triangle.
	* @return HitData, see HitData definition.
	*/
	HitData boundingVolumeVsHull(BoundingVolume* p_Volume, Hull* p_Hull);
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
	/**
	* OBB versus OBB collision test
	* ## SphereVsSphere check happens before 
	* the actual AABBvsAABB collision check. ##
	* @return HitData, see HitData definition.
	*/
	HitData OBBvsOBB(OBB *p_OBB1, OBB *p_OBB2);
	/**
	* OBB versus Sphere collision test
	* Uses Seperating axes test to check for collision
	* @return HitData, see HitData definition.
	*/
	HitData OBBvsSphere(OBB *p_OBB, Sphere *p_Sphere);
	/**
	* OBB versus AABB collision test
	* Uses Seperating axes test to check for collision
	* @return HitData, see HitData definition.
	*/
	HitData OBBvsAABB(OBB *p_OBB, AABB *p_AABB);
	/**
	* Triangle versus Sphere collision test
	* @return HitData, see HitData definition.
	*/
	HitData HullVsSphere(Hull* p_Hull, Sphere *p_Sphere);

private:

	void checkCollisionDepth(float p_RA, float p_RB, float p_R, float &p_Overlap, DirectX::XMFLOAT4 p_L, DirectX::XMFLOAT4 &p_Least);
	HitData seperatingAxisTest(OBB *p_OBB, BoundingVolume *p_vol);
};

