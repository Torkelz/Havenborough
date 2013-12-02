#include "Collision.h"

using namespace DirectX;

Collision::Collision(){}
Collision::~Collision(){}

Collision::HitData Collision::boundingVolumeVsBoundingVolume(BoundingVolume* p_Volume1, BoundingVolume* p_Volume2)
{
	BoundingVolume::Type type = p_Volume2->getType();

	switch(type)
	{		
	case BoundingVolume::Type::AABBOX:
		return boundingVolumeVsAABB(p_Volume1, (AABB*)p_Volume2);

	case BoundingVolume::Type::SPHERE:
		return boundingVolumeVsSphere(p_Volume1, (Sphere*) p_Volume2);

	default:
		HitData hit = HitData();
		return hit;
	}
}

Collision::HitData Collision::boundingVolumeVsSphere(BoundingVolume* p_Volume, Sphere* p_Sphere)
{
	BoundingVolume::Type type = p_Volume->getType();

	switch(type)
	{
	case BoundingVolume::Type::AABBOX:
		return AABBvsSphere((AABB*)p_Volume, p_Sphere);

	case BoundingVolume::Type::SPHERE:
		return sphereVsSphere((Sphere*)p_Volume, p_Sphere);
	default:
		HitData hit = HitData();
		return hit;
	}
}

Collision::HitData Collision::boundingVolumeVsAABB(BoundingVolume* p_Volume, AABB* p_AABB)
{
	BoundingVolume::Type type = p_Volume->getType();
	switch(type)
	{
	case BoundingVolume::Type::AABBOX:
		return AABBvsAABB((AABB*)p_Volume, p_AABB);

	case BoundingVolume::Type::SPHERE:
		return AABBvsSphere(p_AABB, (Sphere*)p_Volume);

	default:
		HitData hit = HitData();
		return hit;
	}
}

Collision::HitData Collision::sphereVsSphere( Sphere* p_Sphere1, Sphere* p_Sphere2 )
{
	HitData hit = HitData();

    XMVECTOR CDiff, vPos;
	CDiff = XMLoadFloat4(p_Sphere2->getPosition());
	vPos = XMLoadFloat4(p_Sphere1->getPosition());
	CDiff = CDiff - vPos;
	
	float c = XMVector3LengthSq(CDiff).m128_f32[0];
	float rSum = p_Sphere2->getRadius() + p_Sphere1->getRadius();
    float rSumSqr = rSum*rSum;

	//Find out if the sphere centers are separated with more distance than the radiuses.
    if(c <= rSumSqr)
	{
		hit.intersect = true;

		XMFLOAT4 position;
		
		XMVECTOR normalized = XMVector4Normalize( XMLoadFloat4(p_Sphere1->getPosition()) - XMLoadFloat4(p_Sphere2->getPosition()));
		XMVECTOR hitPos = normalized * p_Sphere2->getRadius();
		
		position.x = hitPos.m128_f32[0];
		position.y = hitPos.m128_f32[1];
		position.z = hitPos.m128_f32[2];
		position.w = hitPos.m128_f32[3];

		hit.colPos = position;

		hit.colNorm.x = normalized.m128_f32[0];
		hit.colNorm.y = normalized.m128_f32[1];
		hit.colNorm.z = normalized.m128_f32[2];
		hit.colNorm.w = normalized.m128_f32[3];

		hit.colLength = rSum - sqrtf(c);

		hit.colType = Type::SPHEREVSSPHERE;
	}

	return hit;
}

Collision::HitData Collision::AABBvsAABB( AABB* p_AABB1, AABB* p_AABB2 )
{	
	HitData hit = sphereVsSphere(p_AABB1->getSphere(), p_AABB2->getSphere());
	if(!hit.intersect)
		return hit;

	hit = HitData();
	XMFLOAT4* max1 = p_AABB1->getMax();
	XMFLOAT4* min1 = p_AABB1->getMin();
	XMFLOAT4* max2 = p_AABB2->getMax();
	XMFLOAT4* min2 = p_AABB2->getMin();

	//Test if the boxes are separated in any axis.
	if ( min1->x > max2->x || min2->x > max1->x )
		return hit;
	if ( min1->y > max2->y || min2->y > max1->y )
		return hit;
	if ( min1->z > max2->z || min2->z > max1->z )
		return hit;
		
	hit.intersect = true;
	hit.colType = Type::AABBVSAABB;

	return hit;
}

Collision::HitData Collision::AABBvsSphere( AABB* p_AABB, Sphere* p_Sphere )
{
	HitData hit = sphereVsSphere(p_AABB->getSphere(), p_Sphere); 
	if(!hit.intersect)
		return hit;

	hit = HitData();

	//Check to see if the sphere overlaps the AABB
	//const bool AABBOverlapsSphere ( const AABB& B, const SCALAR r, VECTOR& C )
	float s = 0, d = 0; 

	//find the square of the distance
	//from the sphere to the box

	XMFLOAT4* spherePos = p_Sphere->getPosition();
	XMFLOAT3 dist = XMFLOAT3(.0f, .0f, .0f);

	//XMFLOAT4* aabbPos = p_AABB->getPosition();
	//XMFLOAT4* aabbDiagonal = p_AABB->getHalfDiagonal();

	//if the sphere is outside of the box, find the corner closest to the sphere center in each axis.
	//else special case for when the sphere center is inside that axis slab.

	XMFLOAT4* bMin = p_AABB->getMin();
	XMFLOAT4* bMax = p_AABB->getMax();

	// x
	if( spherePos->x <= bMin->x )
	{
		dist.x = bMin->x;
	}
	else if( spherePos->x > bMax->x )
	{
		dist.x = bMax->x;
	}
	else
		dist.x = spherePos->x;

	s = spherePos->x - dist.x;
	d += s*s;

	// y
	if( spherePos->y <= bMin->y )
	{
		dist.y = bMin->y;
	}
	else if( spherePos->y > bMax->y )
	{
		dist.y = bMax->y;
	}
	else
		dist.y = spherePos->y;

	s = spherePos->y - dist.y;
	d += s*s;

	// z
	if( spherePos->z <= bMin->z )
	{
		dist.z = bMin->z;
	}
	else if( spherePos->z > bMax->z )
	{
		dist.z = bMax->z;
	}
	else
		dist.z = spherePos->z;

	s = spherePos->z - dist.z;
	d += s*s;

	if(d <= p_Sphere->getSqrRadius())
	{
		hit.intersect = true;
		hit.colPos.x = dist.x;
		hit.colPos.y = dist.y;
		hit.colPos.z = dist.z;
		hit.colPos.w = 1.f;

		XMVECTOR tempNorm = XMVector4Normalize( XMLoadFloat4(p_Sphere->getPosition()) - XMLoadFloat4(&hit.colPos) );

		XMFLOAT4 colNormPos;
		XMStoreFloat4(&colNormPos, tempNorm);

		hit.colNorm = colNormPos;
		hit.colLength = p_Sphere->getRadius() - sqrtf(d);

		hit.colType = Type::AABBVSSPHERE;
	}

	return hit;
}

//bool AABB::collide(BoundingVolume* p_pVolume)
//{
//	if(p_pVolume->getType() == AABBOX)
//	{
//		return boxVsBox((AABB*)p_pVolume);
//	}
//	else if(p_pVolume->getType() == SPHERE)
//	{
//		return boxVsSphere((Sphere*)p_pVolume);
//	}
//
//	return false;
//}