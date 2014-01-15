#include <boost\test\unit_test.hpp>
#include "..\..\Physics\Source\Collision.h"

BOOST_AUTO_TEST_SUITE(CollisionTest)

BOOST_AUTO_TEST_CASE(SphereVsSphereHit)
{
	Collision col;
	Sphere s1, s2;

	s1 = Sphere(1.f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f));
	s2 = Sphere(100.f, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	BOOST_CHECK_EQUAL(col.sphereVsSphere(&s1, &s2).intersect, true);
}

BOOST_AUTO_TEST_CASE(SphereVsSphereMiss)
{
	Collision col;
	Sphere s1, s2;

	s1 = Sphere(1.f, DirectX::XMFLOAT4(0.0f, 10.0f, 0.0f, 0.0f));
	s2 = Sphere(1.f, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	BOOST_CHECK_EQUAL(col.sphereVsSphere(&s1, &s2).intersect, false);
}

BOOST_AUTO_TEST_CASE(AABBVsSphereHitOrigin)
{
	Collision col;
	Sphere s1;
	AABB aabb;

	HitData hit = HitData();

	s1 = Sphere(1.f, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	aabb = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));

	hit = col.AABBvsSphere(&aabb, &s1);

	BOOST_CHECK_EQUAL(hit.intersect, true);
	BOOST_CHECK_EQUAL(hit.colNorm.x, 0.f);
	BOOST_CHECK_EQUAL(hit.colNorm.y, 0.f);
	BOOST_CHECK_EQUAL(hit.colNorm.z, 0.f);

	BOOST_CHECK_EQUAL(hit.colLength, 100.f);

	BOOST_CHECK_EQUAL(hit.colPos.x, 0.f);
	BOOST_CHECK_EQUAL(hit.colPos.y, 0.f);
	BOOST_CHECK_EQUAL(hit.colPos.z, 0.f);

	BOOST_CHECK_EQUAL((int)hit.colType, (int)Type::AABBVSSPHERE);
}

BOOST_AUTO_TEST_CASE(AABBVsSphereHit)
{
	Collision col;
	Sphere s1;
	AABB aabb;

	HitData hit = HitData();

	s1 = Sphere(1.f, DirectX::XMFLOAT4(0.0f, 2.f, 0.0f, 1.0f));

	aabb = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));

	hit = col.AABBvsSphere(&aabb, &s1);

	BOOST_CHECK_EQUAL(hit.intersect, true);
	BOOST_CHECK_EQUAL(hit.colNorm.x, 0.f);
	BOOST_CHECK_EQUAL(hit.colNorm.y, 1.f);
	BOOST_CHECK_EQUAL(hit.colNorm.z, 0.f);

	BOOST_CHECK_EQUAL(hit.colLength, 0.f);

	BOOST_CHECK_EQUAL(hit.colPos.x, 0.f);
	BOOST_CHECK_EQUAL(hit.colPos.y, 100.f);
	BOOST_CHECK_EQUAL(hit.colPos.z, 0.f);

	BOOST_CHECK_EQUAL((int)hit.colType, (int)Type::AABBVSSPHERE);
}

BOOST_AUTO_TEST_CASE(EXTREMECASE_AABBVsSphereHit)
{
	Collision col;
	Sphere s1;
	AABB aabb;

	s1 = Sphere(0.1f, DirectX::XMFLOAT4(0.0f, 50.0f, 0.0f, 0.0f));

	aabb = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(100.f, 100.f, 100.f, 1.f));

	BOOST_CHECK_EQUAL(col.AABBvsSphere(&aabb, &s1).intersect, true);
}

BOOST_AUTO_TEST_CASE(EXTREMECASE2_AABBVsSphereHit)
{
	Collision col;
	Sphere s1;
	AABB aabb;

	HitData hit = HitData();

	s1 = Sphere(1000.f, DirectX::XMFLOAT4(1001.f, 0.0f, 0.0f, 1.0f));

	aabb = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));

	hit = col.AABBvsSphere(&aabb, &s1);

	BOOST_CHECK_EQUAL(hit.intersect, true);
	BOOST_CHECK_EQUAL(hit.colNorm.x, 1.f);
	BOOST_CHECK_EQUAL(hit.colNorm.y, 0.f);
	BOOST_CHECK_EQUAL(hit.colNorm.z, 0.f);

	BOOST_CHECK_EQUAL(hit.colLength, 0.f);

	BOOST_CHECK_EQUAL(hit.colPos.x, 100.f);
	BOOST_CHECK_EQUAL(hit.colPos.y, 0.f);
	BOOST_CHECK_EQUAL(hit.colPos.z, 0.f);

	BOOST_CHECK_EQUAL((int)hit.colType, (int)Type::AABBVSSPHERE);
}

BOOST_AUTO_TEST_CASE(AABBVsSphereMiss)
{
	Collision col;
	Sphere s1;
	AABB aabb;

	s1 = Sphere(1.f, DirectX::XMFLOAT4(0.0f, 10.0f, 0.0f, 0.0f));

	aabb = AABB(DirectX::XMFLOAT4(-1.f, -1.f, -1.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));

	BOOST_CHECK_EQUAL(col.AABBvsSphere(&aabb, &s1).intersect, false);
}

BOOST_AUTO_TEST_CASE(AABBvsAABBHit)
{
	Collision col;
	AABB aabb, aabbb;

	aabb = AABB(DirectX::XMFLOAT4(-1.f, -1.f, -1.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));
	aabbb = AABB(DirectX::XMFLOAT4(-1.f, -1.f, -1.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));

	BOOST_CHECK_EQUAL(col.AABBvsAABB(&aabb, &aabbb).intersect, true);
}

BOOST_AUTO_TEST_CASE(AABBvsAABBMiss)
{
	Collision col;
	AABB aabb, aabbb;

	aabb = AABB(DirectX::XMFLOAT4(-2.f, -2.f, -2.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));
	aabbb = AABB(DirectX::XMFLOAT4(2.f, 2.f, 2.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));

	BOOST_CHECK_EQUAL(col.AABBvsAABB(&aabb, &aabbb).intersect, false);
}

BOOST_AUTO_TEST_CASE(BoundingVolumeVsBoundingVolumeHit)
{
	Collision col;
	HitData hit = HitData();
	
	Sphere *s1, *s2;
	AABB *aabb1, *aabb2;
	
	s1 = new Sphere(1.f, DirectX::XMFLOAT4(0.0f, 2.5f, 0.0f, 1.0f));
	s2 = new Sphere(2.f, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	aabb1 = new AABB(DirectX::XMFLOAT4(-1.f, -1.f, -1.f, 1.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	aabb2 = new AABB(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f), DirectX::XMFLOAT4(2.f, 2.f, 2.f, 1.f));

	hit = col.boundingVolumeVsBoundingVolume(s1, s2);

	BOOST_CHECK_EQUAL((int)hit.colType, (int)Type::SPHEREVSSPHERE);
	BOOST_CHECK_EQUAL(hit.colNorm.x, 0.f);
	BOOST_CHECK_EQUAL(hit.colNorm.y, 1.f);
	BOOST_CHECK_EQUAL(hit.colNorm.z, 0.f);

	BOOST_CHECK_EQUAL(hit.colPos.x, 0.f);
	BOOST_CHECK_EQUAL(hit.colPos.y, 200.f);
	BOOST_CHECK_EQUAL(hit.colPos.z, 0.f);
	
	BOOST_CHECK_EQUAL(hit.colLength, 50.f);

	delete s1;
	delete s2;
	delete aabb1;
	delete aabb2;
}

BOOST_AUTO_TEST_CASE(OBBvsOBBHit)
{
	Collision col;
	HitData hd;
	DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(0.f, 5.f, 0.f);
	DirectX::XMFLOAT4X4 mtrans;
	DirectX::XMStoreFloat4x4(&mtrans, trans);

	OBB obb1 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(5.f, 5.f, 5.f, 1.f));
	OBB obb2 = OBB(DirectX::XMFLOAT4(0.f, 5.f, 0.f, 1.f), DirectX::XMFLOAT4(5.f, 5.f, 5.f, 1.f));

	hd = col.OBBvsOBB(&obb1, &obb2);
	BOOST_CHECK(hd.intersect);
	
	obb2.updatePosition(mtrans);
	hd = col.OBBvsOBB(&obb1, &obb2);
	BOOST_CHECK(hd.intersect);

	obb2.updatePosition(mtrans);
	hd = col.OBBvsOBB(&obb1, &obb2);
	BOOST_CHECK(!hd.intersect);

	trans = DirectX::XMMatrixTranslation(0.f, -0.5f, 0.f);
	DirectX::XMStoreFloat4x4(&mtrans, trans);

	//A box moving through a box standing still
	for(int i = 0; i < 100; i++)
	{
		hd = col.OBBvsOBB(&obb1, &obb2);
		if(hd.intersect == true)
		{
			BOOST_CHECK_MESSAGE(i >= 10 && i <= 50,"Error on iteration: " + std::to_string(i) + ".\n");
		}
		obb2.updatePosition(mtrans);
	}

	//A rotating box moving through a box standing still.
	obb2 = OBB(DirectX::XMFLOAT4(0.f, 15.f, 0.f, 1.f), DirectX::XMFLOAT4(5.f, 5.f, 5.f, 1.f));
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(3.1415926f/4.f);
	DirectX::XMFLOAT4X4 mrot;
	DirectX::XMStoreFloat4x4(&mrot, rot);
	obb2.setRotationMatrix(mrot);
	for(int i = 0; i < 100; i++)
	{
		hd = col.OBBvsOBB(&obb1, &obb2);
		obb2.updatePosition(mtrans);
		if(hd.intersect == true)
		{
			BOOST_CHECK_MESSAGE(i >= 10 && i <= 50, "Error on rotation iteration: " + std::to_string(i) + ".\n");
		}
	}
}

BOOST_AUTO_TEST_CASE(OBBvsAABBHit)
{
	Collision col;
	HitData hd;
	DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(0.f, 5.f, 0.f);
	DirectX::XMFLOAT4X4 mtrans;
	DirectX::XMStoreFloat4x4(&mtrans, trans);

	OBB obb		= OBB(DirectX::XMFLOAT4(0.f, 5.f, 0.f, 1.f), DirectX::XMFLOAT4(5.f, 5.f, 5.f, 1.f));
	AABB aabb	= AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(5.f, 5.f, 5.f, 1.f));

	hd = col.OBBvsAABB(&obb, &aabb);
	BOOST_CHECK(hd.intersect);
	
	obb.updatePosition(mtrans);
	hd = col.OBBvsAABB(&obb, &aabb);
	BOOST_CHECK(hd.intersect);

	obb.updatePosition(mtrans);
	hd = col.OBBvsAABB(&obb, &aabb);
	BOOST_CHECK(!hd.intersect);

	trans = DirectX::XMMatrixTranslation(0.f, -0.5f, 0.f);
	DirectX::XMStoreFloat4x4(&mtrans, trans);
	for(int i = 0; i < 100; i++)
	{
		hd = col.OBBvsAABB(&obb, &aabb);
		obb.updatePosition(mtrans);
		if(hd.intersect == true)
		{
			BOOST_CHECK_MESSAGE(i >= 10 && i <= 50,"Error on iteration: " + std::to_string(i) + ".\n");
		}
		
	}

	//Rotation
	obb = OBB(DirectX::XMFLOAT4(0.f, 15.f, 0.f, 1.f), DirectX::XMFLOAT4(5.f, 5.f, 5.f, 1.f));
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(3.1415926f/4.f);
	DirectX::XMFLOAT4X4 mrot;
	DirectX::XMStoreFloat4x4(&mrot, rot);
	obb.setRotationMatrix(mrot);
	for(int i = 0; i < 100; i++)
	{
		hd = col.OBBvsAABB(&obb, &aabb);
		obb.updatePosition(mtrans);
		if(hd.intersect == true)
		{
			BOOST_CHECK_MESSAGE(i >= 10 && i <= 50, "Error on rotation iteration LOLZ!: " + std::to_string(i) + ".\n");
		}
	}
}

BOOST_AUTO_TEST_CASE(OBBvsSphereHit)
{
	Collision col;
	HitData hd;
	DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(0.f, 5.f, 0.f);
	DirectX::XMFLOAT4X4 mtrans;
	DirectX::XMStoreFloat4x4(&mtrans, trans);

	OBB obb		= OBB(DirectX::XMFLOAT4(0.f, 5.f, 0.f, 1.f), DirectX::XMFLOAT4(5.f, 5.f, 5.f, 1.f));
	Sphere s	= Sphere(5.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));

	hd = col.OBBvsSphere(&obb, &s);
	BOOST_CHECK(hd.intersect);
	
	obb.updatePosition(mtrans);
	hd = col.OBBvsSphere(&obb, &s);
	BOOST_CHECK(hd.intersect);

	obb.updatePosition(mtrans);
	hd = col.OBBvsSphere(&obb, &s);
	BOOST_CHECK(!hd.intersect);

	trans = DirectX::XMMatrixTranslation(0.f, -0.5f, 0.f);
	DirectX::XMStoreFloat4x4(&mtrans, trans);
	for(int i = 0; i < 100; i++)
	{
		hd = col.OBBvsSphere(&obb, &s);
		obb.updatePosition(mtrans);
		if(hd.intersect == true)
		{
			BOOST_CHECK_MESSAGE(i >= 10 && i <= 50,"OBB Collision Error on iteration: " + std::to_string(i) + ".\n");
		}
		
	}

	//Rotation
	obb = OBB(DirectX::XMFLOAT4(0.f, 15.f, 0.f, 1.f), DirectX::XMFLOAT4(5.f, 5.f, 5.f, 1.f));
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(3.1415926f/4.f);
	DirectX::XMFLOAT4X4 mrot;
	DirectX::XMStoreFloat4x4(&mrot, rot);
	obb.setRotationMatrix(mrot);
	for(int i = 0; i < 100; i++)
	{
		hd = col.OBBvsSphere(&obb, &s);
		obb.updatePosition(mtrans);
		if(hd.intersect == true)
		{
			BOOST_CHECK_MESSAGE(i >= 10 && i <= 50, "OBB Collision Error on rotation iteration: " + std::to_string(i) + ".\n");
		}
	}
}

BOOST_AUTO_TEST_CASE(OBBInsideOBB)
{
	Collision col;
	HitData hd;

	OBB obb1 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f));
	OBB obb2 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1000.f, 1000.f, 1000.f, 1000.f));

	hd = col.OBBvsOBB(&obb1, &obb2);
	BOOST_CHECK(hd.intersect);
}

BOOST_AUTO_TEST_CASE(OBBInsideSphereAndViceVersa)
{
	Collision col;
	HitData hd;

	OBB obb		= OBB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f));
	Sphere s	= Sphere(1000.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	//OBB inside sphere
	hd = col.OBBvsSphere(&obb, &s);
	BOOST_CHECK(hd.intersect);

	obb.setExtent(DirectX::XMFLOAT4(1000.f, 1000.f, 1000.f, 1.f));
	s.setRadius(0.1f);
	///sphere inside OBB
	hd = col.OBBvsSphere(&obb, &s);
	BOOST_CHECK(hd.intersect);
}

BOOST_AUTO_TEST_CASE(OBBInsideAABBAndViceVersa)
{
	Collision col;
	HitData hd;

	OBB obb		= OBB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f));
	AABB aabb	= AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1000.f, 1000.f, 1000.f, 1.f));

	///OBB inside AABB
	hd = col.OBBvsAABB(&obb, &aabb);
	BOOST_CHECK(hd.intersect);

	obb.setExtent(DirectX::XMFLOAT4(1000.f, 1000.f, 1000.f, 1.f));
	aabb = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f));
	///AABB inside OBB
	hd = col.OBBvsAABB(&obb, &aabb);
	BOOST_CHECK(hd.intersect);
}

BOOST_AUTO_TEST_CASE(HullvsSphereHit)
{
		Collision col;
		HitData hd;
		std::vector<Triangle> triangles;
		float size = 1.f;
		triangles.push_back(Triangle(Vector4( -size,  -size, -size, 1.f), Vector4(-size, size, -size, 1.f), Vector4(size,	size, -size, 1.f)));
		triangles.push_back(Triangle(Vector4( -size,  -size, -size, 1.f), Vector4( size, size, -size, 1.f), Vector4(size, -size, -size, 1.f)));

		Hull h  = Hull(triangles);
		Sphere s = Sphere(2.f, DirectX::XMFLOAT4(2.f, 0.f, 0.f, 1.f));
		
		hd = col.HullVsSphere(&h, &s);

		BOOST_CHECK(hd.intersect);

		s.updatePosition(DirectX::XMFLOAT4(2.2f, 0.f, 0.f, 1.f));

		hd = col.HullVsSphere(&h, &s);

		BOOST_CHECK(hd.intersect);

		s.updatePosition(DirectX::XMFLOAT4(3.1f, 0.f, 0.f, 1.f));

		hd = col.HullVsSphere(&h, &s);

		BOOST_CHECK(!hd.intersect);

		DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
		DirectX::XMFLOAT4X4 mtrans;
		DirectX::XMStoreFloat4x4(&mtrans, trans);
		h.updatePosition(mtrans);

		hd = col.HullVsSphere(&h, &s);
		BOOST_CHECK(hd.intersect);
}

BOOST_AUTO_TEST_CASE(HullInBigSphere)
{
	Collision col;
	HitData hd;
	std::vector<Triangle> triangles;
	float size = 1.f;
	triangles.push_back(Triangle(Vector4( -size,  -size, -size, 1.f), Vector4(-size, size, -size, 1.f), Vector4(size,	size, -size, 1.f)));
	triangles.push_back(Triangle(Vector4( -size,  -size, -size, 1.f), Vector4( size, size, -size, 1.f), Vector4(size, -size, -size, 1.f)));

	Hull h  = Hull(triangles);
	Sphere s = Sphere(2000.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f));

	hd = col.HullVsSphere(&h, &s);

	BOOST_CHECK(hd.intersect);
}


BOOST_AUTO_TEST_SUITE_END()