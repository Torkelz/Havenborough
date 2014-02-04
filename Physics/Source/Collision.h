#pragma once
#include "VolumeIncludeAll.h"
#include "..\include\PhysicsTypes.h"

class Collision
{
private:

	struct Plane
	{
		DirectX::XMFLOAT4 normal; //Plane normal
		float	d; //d = dot(n, p) for given point p on plane
		
		/**
		 * Given three noncollinear points (ordered counter clockwise),
		 * to compute a plane in world coordinates.
		 *
		 * @param p_A, first point.
		 * @param p_B, second point.
		 * @param p_C, third point.
		 * @return return the plane that is created.
		 */
		Plane ComputePlane(DirectX::XMVECTOR const &p_A, DirectX::XMVECTOR const &p_B, DirectX::XMVECTOR const &p_C)
		{
			Plane p;
			using DirectX::operator-;
			DirectX::XMStoreFloat4(&p.normal, DirectX::XMVector3Normalize( DirectX::XMVector3Cross(p_B - p_A, p_C - p_A) ));

			p.d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat4(&p.normal), p_A ));

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
	static HitData OBBVsHull(OBB const &p_OBB, Hull const &p_Hull);
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
	static DirectX::XMFLOAT4 findClosestPointOnTriangle(DirectX::XMFLOAT4 const &p_Point, Vector4 const &p_A, Vector4 const &p_B, Vector4 const &p_C);

private:
	static float min(float const &p_A, float const &p_B, float const &p_C);
	static float max(float const &p_A, float const &p_B, float const &p_C);
	static bool SphereVsTriangle(Sphere const &p_Sphere, Triangle const &p_Triangle);

	static bool OBBVsPlane(OBB const &p_OBB, Plane const &p_Plane, DirectX::XMVECTOR &p_Least, float &p_Overlap);
	static bool AABBVsPlane(OBB const &p_OBB, Plane const &p_Plane);

	static void checkCollisionDepth(float p_RA, float p_RB, float p_R, float &p_Overlap, DirectX::XMVECTOR p_L, DirectX::XMVECTOR &p_Least);
	static void checkCollisionDepth(float p_RA, float p_RB, float p_RC, float p_R, float &p_Overlap, DirectX::XMVECTOR p_L, DirectX::XMVECTOR &p_Least);
	static HitData seperatingAxisTest(OBB const &p_OBB, BoundingVolume const &p_vol);

	static bool checkR(float p0, float p1, float p2, float R);
	static bool isZeroVector(DirectX::XMVECTOR p_v);
};

