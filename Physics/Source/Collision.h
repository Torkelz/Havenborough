#pragma once

#include "VolumeIncludeAll.h"
#include "..\include\PhysicsTypes.h"

class Collision
{
private:

	struct Plane
	{
		DirectX::XMFLOAT4 normal;
		float	d;

		/**
		 * Given three noncollinear points (ordered counter clockwise),
		 * to compute a plane in world coordinates.
		 *
		 * @param p_A, first point.
		 * @param p_B, second point.
		 * @param p_C, third point.
		 * @return return the plane that is created.
		 */
		Plane ComputePlane(const DirectX::XMVECTOR &p_A, const DirectX::XMVECTOR &p_B, const DirectX::XMVECTOR &p_C)
		{
			Plane p;
			using DirectX::operator-;
			DirectX::XMStoreFloat4(&p.normal, DirectX::XMVector4Normalize( DirectX::XMVector3Cross(p_B - p_A, p_C - p_A) ));

			p.d = DirectX::XMVectorGetX(DirectX::XMVector4Dot(DirectX::XMLoadFloat4(&p.normal), p_A ));

			return p;
		}
	};

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
	static HitData OBBVsHull(OBB *p_OBB, Hull *p_Hull);
	/**
	* Triangle versus Sphere collision test
	* @return HitData, see HitData definition.
	*/
	static HitData HullVsSphere(Hull const &p_Hull, Sphere const &p_Sphere);

	/**
	* Given point p and the triangle corners in world coordinates, return point in triangle, closest to p
	* @param p_point the point you want to search from
	* @param p_A, corner[0] of triangle in world coordinates
	* @param p_B, corner[1] of triangle in world coordinates
	* @param p_C, corner[2] of triangle in world coordinates
	* @return closest point in the triangle
	*/
	static DirectX::XMFLOAT4 findClosestPointOnTriangle(const DirectX::XMFLOAT4 &p_Point, const Vector4 &p_A, const Vector4 &p_B, const Vector4 &p_C);

private:
	static float min(const float &p_A, const float &p_B, const float &p_C);
	static float max(const float &p_A, const float &p_B, const float &p_C);
	static bool SphereVsTriangle(Sphere *p_Sphere, Triangle *p_Triangle);

	static bool OBBVsPlane(OBB *p_OBB, Plane *p_Plane);

	static void checkCollisionDepth(float p_RA, float p_RB, float p_R, float &p_Overlap, DirectX::XMVECTOR p_L, DirectX::XMVECTOR &p_Least);
	static HitData seperatingAxisTest(OBB const &p_OBB, BoundingVolume const &p_vol);
};

