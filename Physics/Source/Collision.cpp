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
	case BoundingVolume::Type::HULL:
		return boundingVolumeVsHull(p_Volume1, (Hull*)p_Volume2);
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
	case BoundingVolume::Type::HULL:
		return HullVsSphere((Hull*)p_Volume, p_Sphere);
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

HitData Collision::boundingVolumeVsHull(BoundingVolume* p_Volume, Hull* p_Hull)
{
	BoundingVolume::Type type = p_Volume->getType();
	switch(type)
	{
	case BoundingVolume::Type::SPHERE:
			return HullVsSphere(p_Hull, (Sphere*)p_Volume);
	default:
		HitData hit = HitData();
		return hit;
	}
}

HitData Collision::sphereVsSphere( Sphere* p_Sphere1, Sphere* p_Sphere2 )
{
	HitData hit = HitData();

    XMVECTOR CDiff, vPos;	// m
	CDiff = XMLoadFloat4(p_Sphere2->getPosition());
	vPos = XMLoadFloat4(p_Sphere1->getPosition());
	CDiff = CDiff - vPos;
	
	float c = XMVector3LengthSq(CDiff).m128_f32[0];	// m^2
	float rSum = p_Sphere2->getRadius() + p_Sphere1->getRadius();	// m
    float rSumSqr = rSum*rSum;	// m^2

	//Find out if the sphere centers are separated with more distance than the radiuses.
    if(c <= rSumSqr)
	{
		hit.intersect = true;

		XMFLOAT4 position;	// m
		
		XMVECTOR normalized = XMVector4Normalize( XMLoadFloat4(p_Sphere1->getPosition()) - XMLoadFloat4(p_Sphere2->getPosition()));
		XMVECTOR hitPos = normalized * p_Sphere2->getRadius();	// m

		hit.colPos = XMVECTORToVector4(&hitPos) * 100.f;

		hit.colNorm.x = normalized.m128_f32[0];
		hit.colNorm.y = normalized.m128_f32[1];
		hit.colNorm.z = normalized.m128_f32[2];
		hit.colNorm.w = normalized.m128_f32[3];

		hit.colLength = (rSum - sqrtf(c)) * 100.f;

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
	XMFLOAT4* max1 = p_AABB1->getMax();	// m
	XMFLOAT4* min1 = p_AABB1->getMin();	// m
	XMFLOAT4* max2 = p_AABB2->getMax();	// m
	XMFLOAT4* min2 = p_AABB2->getMin();	// m

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
	float s = 0; // m
	float d = 0; // m^2

	//find the square of the distance
	//from the sphere to the box

	XMFLOAT4* spherePos = p_Sphere->getPosition();	// m
	XMFLOAT3 dist = XMFLOAT3(.0f, .0f, .0f);	// m

	//XMFLOAT4* aabbPos = p_AABB->getPosition();
	//XMFLOAT4* aabbDiagonal = p_AABB->getHalfDiagonal();

	//if the sphere is outside of the box, find the corner closest to the sphere center in each axis.
	//else special case for when the sphere center is inside that axis slab.

	XMFLOAT4 bMin;	// m
	XMStoreFloat4( &bMin, XMLoadFloat4( p_AABB->getPosition() ) + XMLoadFloat4(p_AABB->getMin() ));
	XMFLOAT4 bMax;	// m
	XMStoreFloat4( &bMax, XMLoadFloat4( p_AABB->getPosition() ) + XMLoadFloat4(p_AABB->getMax() ));
	// x
	if( spherePos->x <= bMin.x )
	{
		dist.x = bMin.x;
	}
	else if( spherePos->x > bMax.x )
	{
		dist.x = bMax.x;
	}
	else
		dist.x = spherePos->x;

	s = spherePos->x - dist.x;
	d += s*s;

	// y
	if( spherePos->y <= bMin.y )
	{
		dist.y = bMin.y;
	}
	else if( spherePos->y > bMax.y )
	{
		dist.y = bMax.y;
	}
	else
		dist.y = spherePos->y;

	s = spherePos->y - dist.y;
	d += s*s;

	// z
	if( spherePos->z <= bMin.z )
	{
		dist.z = bMin.z;
	}
	else if( spherePos->z > bMax.z )
	{
		dist.z = bMax.z;
	}
	else
		dist.z = spherePos->z;

	s = spherePos->z - dist.z;
	d += s*s;

	if(d <= p_Sphere->getSqrRadius())
	{
		hit.intersect = true;
		hit.colPos.x = dist.x * 100.f;
		hit.colPos.y = dist.y * 100.f;
		hit.colPos.z = dist.z * 100.f;
		hit.colPos.w = 1.f;

		XMFLOAT4 colPos(dist.x, dist.y, dist.z, 1.f);
		XMVECTOR tempNorm = XMVector4Normalize( XMLoadFloat4(p_Sphere->getPosition()) - XMLoadFloat4(&colPos));



		hit.colNorm = XMVECTORToVector4(&tempNorm);
		hit.colLength = (p_Sphere->getRadius() - sqrtf(d)) * 100.f;

		hit.colType = Type::AABBVSSPHERE;
	}

	return hit;
}

HitData Collision::OBBvsOBB(OBB *p_OBB1, OBB *p_OBB2)
{
	HitData hit = sphereVsSphere(&p_OBB1->getSphere(), &p_OBB2->getSphere());
	if(!hit.intersect)
		return hit;

	return seperatingAxisTest(p_OBB1, p_OBB2);
}

HitData Collision::OBBvsSphere(OBB *p_OBB, Sphere *p_Sphere)
{
	HitData hit = sphereVsSphere(&p_OBB->getSphere(), p_Sphere);
	if(!hit.intersect)
		return hit;

	hit = HitData();

	XMFLOAT4 dist;	// m
	

	XMVECTOR sphereCent = XMLoadFloat4(p_Sphere->getPosition());	// m
	
	XMVECTOR closestPoint = XMLoadFloat4(&p_OBB->findClosestPt(*p_Sphere->getPosition()));	// m

	XMVECTOR v = closestPoint - sphereCent;	// m
	XMVECTOR vv = XMVector4Dot(v, v);	// m^2

	if(vv.m128_f32[0] <= p_Sphere->getSqrRadius())
	{
		hit.intersect = true;
		hit.colPos.x = closestPoint.m128_f32[0] * 100.f;
		hit.colPos.y = closestPoint.m128_f32[1] * 100.f;
		hit.colPos.z = closestPoint.m128_f32[2] * 100.f;
		hit.colPos.w = 1.f;

		XMVECTOR tempNorm = XMVector4Normalize(XMLoadFloat4(p_Sphere->getPosition()) - closestPoint);

		hit.colNorm = XMVECTORToVector4(&tempNorm);
		hit.colLength = (p_Sphere->getRadius() - sqrtf(vv.m128_f32[0])) * 100.f;
		hit.colType = Type::OBBVSSPHERE;
	}

	return hit;
}

HitData Collision::OBBvsAABB(OBB *p_OBB, AABB *p_AABB)
{
	HitData hit = sphereVsSphere(&p_OBB->getSphere(), p_AABB->getSphere());
	if(!hit.intersect)
		return hit;

	return seperatingAxisTest(p_OBB, p_AABB);
}

HitData Collision::HullVsSphere(Hull* p_Hull, Sphere* p_Sphere)
{
	HitData hit = sphereVsSphere(&p_Hull->getSphere(), p_Sphere);
	if(!hit.intersect)
		return hit;

	hit = HitData();

	XMVECTOR closestPoint, v, spherePos, point;

	spherePos = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	spherePos = XMLoadFloat4(p_Sphere->getPosition());

	float distance = 10000.f;
	point = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	for(unsigned int i = 0; i < p_Hull->getTriangleListSize(); i++)
	{
		point = XMLoadFloat4(&p_Hull->findClosestPointOnTriangle(*p_Sphere->getPosition(), i));

		v = point - spherePos;

		float vv = XMVector4Dot(v, v).m128_f32[0]; 

		if(vv <= p_Sphere->getSqrRadius())
		{
			hit.intersect = true;
			if(vv <= distance)
			{
				distance = vv;
				closestPoint = point;
			}
		}
	}

	if(hit.intersect)
	{
		hit.colPos.x = closestPoint.m128_f32[0] * 100.f;
		hit.colPos.y = closestPoint.m128_f32[1] * 100.f;
		hit.colPos.z = closestPoint.m128_f32[2] * 100.f;
		hit.colPos.w = 1.f;

		XMVECTOR tempNorm = XMVector4Normalize(XMLoadFloat4(p_Sphere->getPosition()) - closestPoint);

		hit.colNorm = XMVECTORToVector4(&tempNorm);
		hit.colLength = (p_Sphere->getRadius() - sqrtf(distance)) * 100.f;
		hit.colType = Type::HULLVSSPHERE;

	}


	return hit;
}

HitData Collision::seperatingAxisTest(OBB *p_OBB, BoundingVolume *p_vol)
{
	HitData hit = HitData();
	float r, ra, rb, overlap = FLT_MAX;
	const float EPSILON = 0.000001f;
	XMMATRIX R, AbsR;
	XMVECTOR dotResult, dotResult1, dotResult2;
	XMVECTOR a_Center, b_Center, a_Extents, b_Extents, tVec, colPoint; // m
	XMMATRIX a_Axes, b_Axes;
	XMFLOAT4 least;
	a_Center = XMLoadFloat4(p_OBB->getPosition());
	a_Axes = XMLoadFloat4x4(&p_OBB->getAxes());
	a_Extents = XMLoadFloat4(&p_OBB->getExtents()); 

	if(p_vol->getType() == BoundingVolume::Type::OBB)
	{
		
		b_Center = XMLoadFloat4(((OBB*)p_vol)->getPosition());
		b_Axes = XMLoadFloat4x4(&((OBB*)p_vol)->getAxes());
		b_Extents = XMLoadFloat4(&((OBB*)p_vol)->getExtents());
	}
	else
	{
		b_Center = XMLoadFloat4(((AABB*)p_vol)->getPosition());
		b_Axes = XMMatrixIdentity();
		b_Extents = XMLoadFloat4(((AABB*)p_vol)->getHalfDiagonal());
	}
	//Compute rotation matrix expressing b in a's coordinate frame
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			dotResult = XMVector3Dot(a_Axes.r[i], b_Axes.r[j]);
			R.r[i].m128_f32[j] = dotResult.m128_f32[0];
		}
	}

	// Compute translation vector t
	XMVECTOR t = b_Center - a_Center;	// m
	tVec = t;
	
	// Bring translation into a’s coordinate frame
	dotResult = XMVector3Dot(t, a_Axes.r[0]); 
	dotResult1 = XMVector3Dot(t, a_Axes.r[1]); 
	dotResult2 = XMVector3Dot(t, a_Axes.r[2]); 
	t = XMVectorSet(dotResult.m128_f32[0], dotResult1.m128_f32[0], dotResult2.m128_f32[0], 0.f);

	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null 
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			AbsR.r[i].m128_f32[j] = fabs(R.r[i].m128_f32[j]) + EPSILON;
		}
	}

	XMVECTOR LA[] = { XMVectorSet(a_Extents.m128_f32[0], 0.f, 0.f, 0.f),
					  XMVectorSet(0.f, a_Extents.m128_f32[1], 0.f, 0.f),
					  XMVectorSet(0.f, 0.f, a_Extents.m128_f32[2], 0.f), };

	
	XMVECTOR LB[] = { XMVectorSet(b_Extents.m128_f32[0], 0.f, 0.f, 0.f),
					  XMVectorSet(0.f, b_Extents.m128_f32[1], 0.f, 0.f),
					  XMVectorSet(0.f, 0.f, b_Extents.m128_f32[2], 0.f), };

	XMVECTOR L = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	
	XMFLOAT4 cross;
	XMFLOAT4 storeL;

	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++) 
	{
		ra = a_Extents.m128_f32[i]; 
		rb = b_Extents.m128_f32[0]	* AbsR.r[i].m128_f32[0] + b_Extents.m128_f32[1] * AbsR.r[i].m128_f32[1] + b_Extents.m128_f32[2] * AbsR.r[i].m128_f32[2];
		float r = t.m128_f32[i];
		if(fabs(r) > ra + rb)
			return hit;

		colPoint.m128_f32[i] = r*0.5f;
		XMStoreFloat4(&storeL, LA[i]);
		checkCollisionDepth(ra, rb, r, overlap, storeL, least);
		
	}

	//Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++) 
	{
		ra = a_Extents.m128_f32[0]	* AbsR.r[0].m128_f32[i] + a_Extents.m128_f32[1] * AbsR.r[1].m128_f32[i] + a_Extents.m128_f32[2] * AbsR.r[2].m128_f32[i];
		rb = b_Extents.m128_f32[i]; 
		r  = t.m128_f32[0] * R.r[0].m128_f32[i] + t.m128_f32[1] * R.r[1].m128_f32[i] + t.m128_f32[2] * R.r[2].m128_f32[i];
		if(fabs(r) > ra + rb)
			return hit;
		XMStoreFloat4(&storeL, LB[i]);
		checkCollisionDepth(ra, rb, r, overlap, storeL, least);
	}

	// Test axis L = A0 x B0
	ra		= a_Extents.m128_f32[1]	* AbsR.r[2].m128_f32[0] + a_Extents.m128_f32[2] * AbsR.r[1].m128_f32[0];
	rb		= b_Extents.m128_f32[1]	* AbsR.r[0].m128_f32[2] + b_Extents.m128_f32[2] * AbsR.r[0].m128_f32[1];
	r		= t.m128_f32[2] * R.r[1].m128_f32[0] - t.m128_f32[1] * R.r[2].m128_f32[0];
	if (fabs(r) > ra + rb)
		return hit;

	L = XMVector3Cross(LA[0], LB[0]);
	XMStoreFloat4(&storeL, L);
	checkCollisionDepth(ra, rb, r, overlap, storeL, least);

	// Test axis L = A0 x B1
	ra		= a_Extents.m128_f32[1]	* AbsR.r[2].m128_f32[1] + a_Extents.m128_f32[2] * AbsR.r[1].m128_f32[1];
	rb		= b_Extents.m128_f32[0]	* AbsR.r[0].m128_f32[2] + b_Extents.m128_f32[2] * AbsR.r[0].m128_f32[0];
	r		= t.m128_f32[2] * R.r[1].m128_f32[1] - t.m128_f32[1] * R.r[2].m128_f32[1];
	if (fabs(r) > ra + rb) 
		return hit;

	L = XMVector3Cross(LA[0], LB[1]);
	XMStoreFloat4(&storeL, L);
	checkCollisionDepth(ra, rb, r, overlap, storeL, least);

	// Test axis L = A0 x B2
	ra		= a_Extents.m128_f32[1]	* AbsR.r[2].m128_f32[2] + a_Extents.m128_f32[2] * AbsR.r[1].m128_f32[2];
	rb		= b_Extents.m128_f32[0]	* AbsR.r[0].m128_f32[1] + b_Extents.m128_f32[1] * AbsR.r[0].m128_f32[0];
	r		= t.m128_f32[2] * R.r[1].m128_f32[2] - t.m128_f32[1] * R.r[2].m128_f32[2];
	if (fabs(r) > ra + rb) 
		return hit;

	L = XMVector3Cross(LA[0], LB[2]);
	XMStoreFloat4(&storeL, L);
	checkCollisionDepth(ra, rb, r, overlap, storeL, least);

	// Test axis L = A1 x B0
	ra		= a_Extents.m128_f32[0]	* AbsR.r[2].m128_f32[0] + a_Extents.m128_f32[2] * AbsR.r[0].m128_f32[0];
	rb		= b_Extents.m128_f32[1]	* AbsR.r[1].m128_f32[2] + b_Extents.m128_f32[2] * AbsR.r[1].m128_f32[1];
	r		= t.m128_f32[0] * R.r[2].m128_f32[0] - t.m128_f32[2] * R.r[0].m128_f32[0];
	if (fabs(r) > ra + rb) 
		return hit;

	L = XMVector3Cross(LA[1], LB[0]);
	XMStoreFloat4(&storeL, L);
	checkCollisionDepth(ra, rb, r, overlap, storeL, least);

	// Test axis L = A1 x B1
	ra		= a_Extents.m128_f32[0]	* AbsR.r[2].m128_f32[1] + a_Extents.m128_f32[2] * AbsR.r[0].m128_f32[1];
	rb		= b_Extents.m128_f32[0]	* AbsR.r[1].m128_f32[2] + b_Extents.m128_f32[2] * AbsR.r[1].m128_f32[0];
	r		= t.m128_f32[0] * R.r[2].m128_f32[1] - t.m128_f32[2] * R.r[0].m128_f32[1];
	if (fabs(r) > ra + rb) 
		return hit;

	L = XMVector3Cross(LA[1], LB[1]);
	XMStoreFloat4(&storeL, L);
	checkCollisionDepth(ra, rb, r, overlap, storeL, least);

	// Test axis L = A1 x B2
	ra		= a_Extents.m128_f32[0]	* AbsR.r[2].m128_f32[2] + a_Extents.m128_f32[2] * AbsR.r[0].m128_f32[2];
	rb		= b_Extents.m128_f32[0]	* AbsR.r[1].m128_f32[1] + b_Extents.m128_f32[1] * AbsR.r[1].m128_f32[0];
	r		= t.m128_f32[0] * R.r[2].m128_f32[2] - t.m128_f32[2] * R.r[0].m128_f32[2];
	if (fabs(r) > ra + rb) 
		return hit;

	L = XMVector3Cross(LA[1], LB[2]);
	XMStoreFloat4(&storeL, L);
	checkCollisionDepth(ra, rb, r, overlap, storeL, least);

	// Test axis L = A2 x B0 
	ra		= a_Extents.m128_f32[0]	* AbsR.r[1].m128_f32[0] + a_Extents.m128_f32[1] * AbsR.r[0].m128_f32[0];
	rb		= b_Extents.m128_f32[1]	* AbsR.r[2].m128_f32[2] + b_Extents.m128_f32[2] * AbsR.r[2].m128_f32[1];
	r		= t.m128_f32[1] * R.r[0].m128_f32[0] - t.m128_f32[0] * R.r[1].m128_f32[0];
	if(fabs(r) > ra + rb)
		return hit;

	L = XMVector3Cross(LA[2], LB[0]);
	XMStoreFloat4(&storeL, L);
	checkCollisionDepth(ra, rb, r, overlap, storeL, least);

	// Test axis L = A2 x B1
	ra		= a_Extents.m128_f32[0]	* AbsR.r[1].m128_f32[1] + a_Extents.m128_f32[1] * AbsR.r[0].m128_f32[1];
	rb		= b_Extents.m128_f32[0]	* AbsR.r[2].m128_f32[2] + b_Extents.m128_f32[2] * AbsR.r[2].m128_f32[0];
	r		= t.m128_f32[1] * R.r[0].m128_f32[1] - t.m128_f32[0] * R.r[1].m128_f32[1];
	if (fabs(r) > ra + rb) 
		return hit;

	L = XMVector3Cross(LA[2], LB[1]);
	XMStoreFloat4(&storeL, L);
	checkCollisionDepth(ra, rb, r, overlap, storeL, least);

	// Test axis L = A2 x B2
	ra		= a_Extents.m128_f32[0]	* AbsR.r[1].m128_f32[2] + a_Extents.m128_f32[1] * AbsR.r[0].m128_f32[2];
	rb		= b_Extents.m128_f32[0]	* AbsR.r[2].m128_f32[1] + b_Extents.m128_f32[1] * AbsR.r[2].m128_f32[0];
	r		= t.m128_f32[1] * R.r[0].m128_f32[2] - t.m128_f32[0] * R.r[1].m128_f32[2];
	if (fabs(r) > ra + rb) 
		return hit;

	L = XMVector3Cross(LA[2], LB[2]);
	XMStoreFloat4(&storeL, L);
	checkCollisionDepth(ra, rb, r, overlap, storeL, least);
	
	
	XMVECTOR v = XMLoadFloat4(&least);
	
	hit.intersect = true;

	float temp = XMVector4Dot(tVec, v).m128_f32[0];

	if(temp > 0)
		v *= -1.f;

	hit.colNorm = XMVECTORToVector4(&XMVector4Normalize(v));
	hit.colLength = overlap * 100.f;
	if(p_vol->getType() == BoundingVolume::Type::OBB)
		hit.colType = Type::OBBVSOBB;
	else
		hit.colType = Type::OBBVSAABB;


 	return hit;
}

void Collision::checkCollisionDepth(float p_RA, float p_RB, float p_R, float &p_Overlap, XMFLOAT4 p_L, XMFLOAT4 &p_Least)
{
	float lLength = XMVector4LengthSq(XMLoadFloat4(&p_L)).m128_f32[0];
	if(lLength != 0.0f)
	{
		p_R = (p_R - (p_RA + p_RB))/lLength;
		if(p_Overlap > fabs(p_R))
		{
			p_Overlap = fabs(p_R);
			p_Least = p_L;
		}
	}
}