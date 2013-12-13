#include "Collision.h"

using namespace DirectX;

Collision::Collision(){}
Collision::~Collision(){}

HitData Collision::boundingVolumeVsBoundingVolume(BoundingVolume* p_Volume1, BoundingVolume* p_Volume2)
{
	BoundingVolume::Type type = p_Volume2->getType();

	switch(type)
	{		
	case BoundingVolume::Type::AABBOX:
		return boundingVolumeVsAABB(p_Volume1, (AABB*)p_Volume2);
	case BoundingVolume::Type::SPHERE:
		return boundingVolumeVsSphere(p_Volume1, (Sphere*) p_Volume2);
	case BoundingVolume::Type::OBB:
		return boundingVolumeVsOBB(p_Volume1, (OBB*)p_Volume2);
	default:
		HitData hit = HitData();
		return hit;
	}
}

HitData Collision::boundingVolumeVsSphere(BoundingVolume* p_Volume, Sphere* p_Sphere)
{
	BoundingVolume::Type type = p_Volume->getType();

	switch(type)
	{
	case BoundingVolume::Type::AABBOX:
		return AABBvsSphere((AABB*)p_Volume, p_Sphere);
	case BoundingVolume::Type::SPHERE:
		return sphereVsSphere((Sphere*)p_Volume, p_Sphere);
	case BoundingVolume::Type::OBB:
		return OBBvsSphere((OBB*)p_Volume, p_Sphere);
	default:
		HitData hit = HitData();
		return hit;
	}
}

HitData Collision::boundingVolumeVsAABB(BoundingVolume* p_Volume, AABB* p_AABB)
{
	BoundingVolume::Type type = p_Volume->getType();
	switch(type)
	{
	case BoundingVolume::Type::AABBOX:
		return AABBvsAABB((AABB*)p_Volume, p_AABB);
	case BoundingVolume::Type::SPHERE:
		return AABBvsSphere(p_AABB, (Sphere*)p_Volume);
	case BoundingVolume::Type::OBB:
		return OBBvsAABB((OBB*)p_Volume, p_AABB);
	default:
		HitData hit = HitData();
		return hit;
	}
}

HitData Collision::boundingVolumeVsOBB(BoundingVolume* p_Volume, OBB* p_OBB)
{
	BoundingVolume::Type type = p_Volume->getType();
	switch(type)
	{
	case BoundingVolume::Type::AABBOX:
		return OBBvsAABB(p_OBB, (AABB*)p_Volume);
	case BoundingVolume::Type::SPHERE:
		return OBBvsSphere(p_OBB, (Sphere*)p_Volume);
	case BoundingVolume::Type::OBB:
		return OBBvsOBB((OBB*)p_Volume, p_OBB);
	default:
		HitData hit = HitData();
		return hit;
	}
}

HitData Collision::sphereVsSphere( Sphere* p_Sphere1, Sphere* p_Sphere2 )
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
		
		//position.x = hitPos.m128_f32[0];
		//position.y = hitPos.m128_f32[1];
		//position.z = hitPos.m128_f32[2];
		//position.w = hitPos.m128_f32[3];

		hit.colPos = XMVECTORToVector4(&hitPos);;

		hit.colNorm.x = normalized.m128_f32[0];
		hit.colNorm.y = normalized.m128_f32[1];
		hit.colNorm.z = normalized.m128_f32[2];
		hit.colNorm.w = normalized.m128_f32[3];

		hit.colLength = rSum - sqrtf(c);

		hit.colType = Type::SPHEREVSSPHERE;
	}

	return hit;
}

HitData Collision::AABBvsAABB( AABB* p_AABB1, AABB* p_AABB2 )
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

HitData Collision::AABBvsSphere( AABB* p_AABB, Sphere* p_Sphere )
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

		XMVECTOR tempNorm = XMVector4Normalize( XMLoadFloat4(p_Sphere->getPosition()) - Vector4ToXMVECTOR(&hit.colPos) );

		/*XMFLOAT4 colNormPos;
		XMStoreFloat4(&colNormPos, tempNorm);*/

		hit.colNorm = XMVECTORToVector4(&tempNorm);
		hit.colLength = p_Sphere->getRadius() - sqrtf(d);

		hit.colType = Type::AABBVSSPHERE;
	}

	return hit;
}

HitData Collision::OBBvsOBB(OBB *p_OBB1, OBB *p_OBB2)
{
	HitData hit = HitData();

	XMMATRIX invRotA, invRotB;
	
	XMVECTOR sizeA = XMLoadFloat3(&p_OBB1->getExtent());
	XMVECTOR sizeB = XMLoadFloat3(&p_OBB2->getExtent());

	XMVECTOR vCenterPos1 = XMLoadFloat4(p_OBB1->getPosition());
	XMVECTOR vCenterPos2 = XMLoadFloat4(p_OBB2->getPosition());
	
	invRotA = XMLoadFloat4x4(&p_OBB1->getInvRotation());
	invRotB = XMLoadFloat4x4(&p_OBB2->getInvRotation());

	XMMATRIX R, AR;
	XMVECTOR dotResult0, dotResult1, dotResult2;
	float extentA, extentB, separation;

    // Calculate B to A rotation matrix
	for(int i = 0; i < 3; i++ )
	{
       for(int k = 0; k < 3; k++ )
		{
			dotResult0 = XMVector4Dot(invRotA.r[i], invRotB.r[k]);
			R.r[i].m128_f32[k] =  dotResult0.m128_f32[0];
			AR.r[i].m128_f32[k] = fabs(R.r[i].m128_f32[k]);
        }
	}

	// Vector separating the centers of Box B and of Box A	
	XMVECTOR vSepWS = vCenterPos2 - vCenterPos1;
	// Rotated into Box A's coordinates
	dotResult0 = XMVector4Dot(vSepWS, invRotA.r[0]);
	dotResult1 = XMVector4Dot(vSepWS, invRotA.r[1]);
	dotResult2 = XMVector4Dot(vSepWS, invRotA.r[2]);
	
	XMVECTOR vSepA	 = XMVectorSet(dotResult0.m128_f32[0], dotResult1.m128_f32[0], dotResult2.m128_f32[0], 0.f);
            
     // Test if any of A's basis vectors separate the box
	XMVECTOR temp;
	for(int i = 0; i < 3; i++ )
	{
		extentA = sizeA.m128_f32[i];
		temp = XMVectorSet(AR.r[i].m128_f32[0], AR.r[i].m128_f32[1], AR.r[i].m128_f32[2], 0.f);
		dotResult0 = XMVector4Dot(sizeB, temp);
		extentB = dotResult0.m128_f32[0];
		separation = fabs(vSepA.m128_f32[i]);

		if(separation > extentA + extentB)
		{
			//No intersection
			hit.intersect = false;
			return hit;
		}
	}

	// Test if any of B's basis vectors separate the box
	for(int i = 0; i < 3; i++)
	{
		extentB = sizeA.m128_f32[i];
		temp = XMVectorSet(AR.r[0].m128_f32[i], AR.r[1].m128_f32[i], AR.r[2].m128_f32[i], 0.f);
		dotResult0 = XMVector4Dot(sizeA, temp);
		extentA = dotResult0.m128_f32[0];

		temp = XMVectorSet(R.r[0].m128_f32[i], R.r[1].m128_f32[i], R.r[2].m128_f32[i], 0.f);
		dotResult0 = XMVector4Dot(vSepA, temp);

		separation = fabs(dotResult0.m128_f32[0]);

		if(separation > extentA + extentB)
		{
			//No intersection
			hit.intersect = false;
			return hit;
		}
	}

	//// Now test Cross Products of each basis vector combination ( A[i], B[k] )
	for(int i = 0; i < 3; i++)
	{
		for(int k = 0; k < 3; k++)
		{
			int i1 = (i+1)%3, i2 = (i+2)%3;
			int k1 = (k+1)%3, k2 = (k+2)%3;
			extentA = sizeA.m128_f32[i1] * AR.r[i2].m128_f32[k]  +  sizeA.m128_f32[i2] * AR.r[i1].m128_f32[k];
			extentB = sizeB.m128_f32[k1] * AR.r[i].m128_f32[k2]  +  sizeB.m128_f32[k2] * AR.r[i].m128_f32[k1];
			separation = fabs( vSepA.m128_f32[i2] * R.r[i1].m128_f32[k]  -  vSepA.m128_f32[i1] * R.r[i2].m128_f32[k] );
			if( separation > extentA + extentB )
			{
				hit.intersect = false;
				return hit;
			}
		}
	}

	//// No separating axis found, the boxes overlap
	hit.intersect = true;
	hit.colType = Type::OBBVSOBB;
	//Calculate intersection point()
	return hit;
}

HitData Collision::OBBvsSphere(OBB *p_OBB, Sphere *p_Sphere)
{
	HitData hit = HitData();
	float fDist;
	float fDistSq		= 0;
	XMVECTOR extent		= XMLoadFloat3(&p_OBB->getExtent());
	XMVECTOR spherePos	= XMLoadFloat4(p_Sphere->getPosition());
	XMMATRIX invRot		= XMLoadFloat4x4(&p_OBB->getInvRotation());

	float fRadius = p_Sphere->getRadius();
	
	XMVECTOR P = XMVector3Transform(spherePos, invRot);
	
	// Add distance squared from sphere centerpoint to box for each axis
	for ( int i = 0; i < 3; i++ )
	{
		if ( fabs(P.m128_f32[i]) > extent.m128_f32[i] )
		{
			fDist = fabs(P.m128_f32[i]) - extent.m128_f32[i];
			fDistSq += fDist*fDist;
		}
	}

	if( fDistSq <= fRadius*fRadius )
	{
		hit.intersect = true;
		hit.colType = Type::OBBVSSPHERE;
		//Calculate pos
	}

	return hit;
}

HitData Collision::OBBvsAABB(OBB *p_OBB, AABB *p_AABB)
{
	HitData Hit;
	return Hit;
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