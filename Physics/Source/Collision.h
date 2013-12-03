#pragma once

#include "VolumeIncludeAll.h"
#include "..\include\PhysicsTypes.h"

inline DirectX::XMFLOAT3 vector3ToXMFLOAT3(Vector3* p_vector)
{
	DirectX::XMFLOAT3 vector;
	vector = DirectX::XMFLOAT3(p_vector->x, p_vector->y, p_vector->z);

	return vector;
};

inline DirectX::XMFLOAT4 vector4ToXMFLOAT4(Vector4* p_vector)
{
	DirectX::XMFLOAT4 vector = DirectX::XMFLOAT4(p_vector->x, p_vector->y, p_vector->z, p_vector->w);

	return vector;
};

inline Vector3 XMFLOAT3ToVector3(DirectX::XMFLOAT3* p_vector)
{
	Vector3 vector = Vector3(p_vector->x, p_vector->y, p_vector->z);

	return vector;
};

inline Vector4 XMFLOAT4ToVector4(DirectX::XMFLOAT4* p_vector)
{
	Vector4 vector = Vector4(p_vector->x, p_vector->y, p_vector->z, p_vector->w);

	return vector;
};

inline DirectX::XMVECTOR Vector4ToXMVECTOR(Vector4* p_vector)
{
	DirectX::XMVECTOR vector = DirectX::XMVectorSet(p_vector->x, p_vector->y, p_vector->z, p_vector->w);

	return vector;
};

inline Vector4 XMVECTORToVector4(DirectX::XMVECTOR* p_vector)
{
	Vector4 vector = Vector4(p_vector->m128_f32[0], p_vector->m128_f32[1], p_vector->m128_f32[2], p_vector->m128_f32[3]);

	return vector;
};

class Collision
{
public:
	static enum { OUTSIDE = 0, INTERSECT, INSIDE };

	//enum class Type
	//{
	//	NONE,
	//	SPHEREVSSPHERE,
	//	AABBVSSPHERE,
	//	AABBVSAABB
	//};

	//struct HitData
	//{
	//	DirectX::XMFLOAT4	colPos;
	//	DirectX::XMFLOAT4	colNorm;
	//	bool				intersect;
	//	Collision::Type		colType;
	//	float				colLength;

	//	HitData()
	//	{
	//		colPos		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	//		colNorm		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	//		intersect	= false;
	//		colType		= Collision::Type::NONE;
	//		colLength	= -1.f;
	//	}
	//};

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

