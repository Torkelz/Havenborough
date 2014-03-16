#pragma once
#include "VolumeIncludeAll.h"
#include "..\include\PhysicsTypes.h"

class Collision
{
public:
	/**
	* Redirect to the appropriate check, when neither BoundingVolumes' type is known.
	* @return HitData, see HitData definition.
	*/
	static HitData boundingVolumeVsBoundingVolume(BoundingVolume const &p_Volume1, BoundingVolume const &p_Volume2);
	/**
	* Check for the appropriate collision, a BoundingVolume versus a sphere.
	* @return HitData, see HitData definition.
	*/
	static HitData boundingVolumeVsSphere(BoundingVolume const &p_Volume, Sphere const &p_Sphere);
	/**
	* Check for the appropriate collision, a BoundingVolume versus an AABB.
	* @return HitData, see HitData definition.
	*/
	static HitData boundingVolumeVsAABB(BoundingVolume const &p_Volume, const AABB &p_AABB);
	/**
	* Check for the appropriate collision, a BoundingVolume versus an OBB.
	* @return HitData, see HitData definition.
	*/
	static HitData boundingVolumeVsOBB(BoundingVolume const &p_Volume, const OBB &p_OBB);
	/**
	* Check for the appropriate collision, a BoundingVolume versus a Triangle.
	* @return HitData, see HitData definition.
	*/
	static HitData boundingVolumeVsHull(BoundingVolume const &p_Volume, Hull const &p_Hull);
	
	static bool Collision::surroundingSphereVsSphere(Sphere const &p_Sphere1, Sphere const &p_Sphere2);

	/**
	* Sphere versus Sphere collision
	* @return HitData, see HitData definition.
	*/
	static HitData sphereVsSphere(Sphere const &p_Sphere1, const Sphere &p_Sphere2);
	/**
	* AABB versus AABB collision
	* ## SphereVsSphere check happens before 
	* the actual AABBvsAABB collision check. ##
	* @return HitData, see HitData definition.
	*/
	static HitData AABBvsAABB(AABB const &p_AABB1, const AABB &p_AABB2 );
	/**
	* AABB versus Sphere collision
	* ## SphereVsSphere check happens before 
	* the actual AABBvsAABB collision check. ##
	* @return HitData, see HitData definition.
	*/
	static HitData AABBvsSphere(AABB const &p_AABB, Sphere const &p_Sphere );

	static bool AABBvsSphereIntersect(DirectX::XMFLOAT4 p_Min, DirectX::XMFLOAT4 p_Max, Sphere const &p_Sphere);

	static bool AABBInsideSphere(DirectX::XMFLOAT4 p_Min, DirectX::XMFLOAT4 p_Max, Sphere const &p_Sphere);

	static bool SphereInsideAABB(DirectX::XMFLOAT4 p_Min, DirectX::XMFLOAT4 p_Max, Sphere const &p_Sphere);
	/**
	* OBB versus OBB collision test
	* ## SphereVsSphere check happens before 
	* the actual AABBvsAABB collision check. ##
	* @return HitData, see HitData definition.
	*/
	static HitData OBBvsOBB(OBB const &p_OBB1, OBB const &p_OBB2);
	/**
	* OBB versus Sphere collision test
	* Uses Seperating axes test to check for collision
	* @return HitData, see HitData definition.
	*/
	static HitData OBBvsSphere(OBB const &p_OBB, Sphere const &p_Sphere);
	/**
	* OBB versus AABB collision test
	* Uses Seperating axes test to check for collision
	* @return HitData, see HitData definition.
	*/
	static HitData OBBvsAABB(OBB const &p_OBB, AABB const &p_AABB);

	/**
	 * OBB Versus Hull collision test
	 * Uses seperating axis test to check for collision
	 * @return HitData, see HitData definition.
	 */
	static HitData OBBVsHull(OBB const &p_OBB, Hull const &p_Hull);
	/**
	* Triangle versus Sphere collision test
	* @return HitData, see HitData definition.
	*/
	static HitData HullVsSphere(Hull const &p_Hull, Sphere const &p_Sphere);

private:
	static HitData SATBoxVsBox(OBB const &p_OBB, BoundingVolume const &p_vol);
	static HitData SATBoxVsHull(OBB const &p_OBB, Hull const &p_Hull);
	static void checkCollisionDepth(float p_RA, float p_RB, float p_R, float &p_Overlap, DirectX::XMVECTOR p_L, DirectX::XMVECTOR &p_Least);
	static bool checkCollision(DirectX::XMVECTOR p_Axis, float p_TriangleProjection0, float p_TriangleProjection1,
							   float p_BoxProjection, float &p_Overlap, DirectX::XMVECTOR &p_Least);
};

