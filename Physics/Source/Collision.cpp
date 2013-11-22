#include "Collision.h"

using namespace DirectX;

Collision::Collision(void){}
Collision::~Collision(void){}

bool Collision::sphereVsSphere( Sphere* p_sphere1, Sphere* p_sphere2 )
{
    XMVECTOR CDiff, vPos;
	CDiff = XMLoadFloat4(p_sphere2->getPosition());
	vPos = XMLoadFloat4(p_sphere1->getPosition());
	CDiff = CDiff - vPos;
	
	float c = XMVector3LengthSq(CDiff).m128_f32[0];
	float rSum = p_sphere2->getRadius() + p_sphere1->getRadius();
    float rSumSqr = rSum*rSum;

	//Find out if the sphere centers are separated with more distance than the radiuses.
    return c <= rSumSqr;
}

bool Collision::AABBvsAABB( AABB* p_aabb1, AABB* p_aabb2 )
{
	XMFLOAT4* max1 = p_aabb1->getMax();
	XMFLOAT4* min1 = p_aabb1->getMin();
	XMFLOAT4* max2 = p_aabb2->getMax();
	XMFLOAT4* min2 = p_aabb2->getMin();

	//Test if the boxes are separated in any axis.
	if ( min1->x > max2->x || min2->x > max1->x )
		return false;
	if ( min1->y > max2->y || min2->y > max1->y )
		return false;
	if ( min1->z > max2->z || min2->z > max1->z )
		return false;

	return true;
}

bool Collision::AABBvsSphere( AABB* p_aabb, Sphere* p_sphere )
{
	//Check to see if the sphere overlaps the AABB
	//const bool AABBOverlapsSphere ( const AABB& B, const SCALAR r, VECTOR& C )
	float s = 0, d = 0; 

	//find the square of the distance
	//from the sphere to the box

	XMFLOAT4* spherePos = p_sphere->getPosition();
	XMFLOAT3 dist = XMFLOAT3(.0f, .0f, .0f);
	XMFLOAT4* aabbPos = p_aabb->getPosition();
	XMFLOAT4* aabbDiagonal = p_aabb->getHalfDiagonal();

	//if the sphere is outside of the box, find the corner closest to the sphere center in each axis.
	//else special case for when the sphere center is inside that axis slab.

	// x
	if( spherePos->x - aabbPos->x <= -aabbDiagonal->x )
	{
		dist.x = -aabbDiagonal->x;
	}
	else if( spherePos->x - aabbPos->x > aabbDiagonal->x )
	{
		dist.x = aabbDiagonal->x;
	}
	else
		dist.x = spherePos->x;

	s = spherePos->x - dist.x;
	d += s*s;

	// y
	if( spherePos->y - aabbPos->y <= -aabbDiagonal->y )
	{
		dist.y = -aabbDiagonal->y;
	}
	else if( spherePos->y - aabbPos->y > aabbDiagonal->y )
	{
		dist.y = aabbDiagonal->y;
	}
	else
		dist.y = spherePos->y;

	s = spherePos->y - dist.y;
	d += s*s;

	// z
	if( spherePos->z - aabbPos->z <= -aabbDiagonal->z )
	{
		dist.z = -aabbDiagonal->z;
	}
	else if( spherePos->z - aabbPos->z > aabbDiagonal->z )
	{
		dist.z = aabbDiagonal->z;
	}
	else
		dist.z = spherePos->z;

	s = spherePos->z - dist.z;
	d += s*s;

	return d <= p_sphere->getSqrRadius();
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