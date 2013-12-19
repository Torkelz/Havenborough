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

	aabb = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));

	hit = col.AABBvsSphere(&aabb, &s1);

	BOOST_CHECK_EQUAL(hit.intersect, true);
	BOOST_CHECK_EQUAL(hit.colNorm.x, 0.f);
	BOOST_CHECK_EQUAL(hit.colNorm.y, 0.f);
	BOOST_CHECK_EQUAL(hit.colNorm.z, 0.f);

	BOOST_CHECK_EQUAL(hit.colLength, 1.f);

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
	BOOST_CHECK_EQUAL(hit.colPos.y, 1.f);
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

	aabb = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));

	hit = col.AABBvsSphere(&aabb, &s1);

	BOOST_CHECK_EQUAL(hit.intersect, true);
	BOOST_CHECK_EQUAL(hit.colNorm.x, 1.f);
	BOOST_CHECK_EQUAL(hit.colNorm.y, 0.f);
	BOOST_CHECK_EQUAL(hit.colNorm.z, 0.f);

	BOOST_CHECK_EQUAL(hit.colLength, 0.f);

	BOOST_CHECK_EQUAL(hit.colPos.x, 1.f);
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
	BOOST_CHECK_EQUAL(hit.colPos.y, 2.f);
	BOOST_CHECK_EQUAL(hit.colPos.z, 0.f);
	
	BOOST_CHECK_EQUAL(hit.colLength, 0.5f);

	delete s1;
	delete s2;
	delete aabb1;
	delete aabb2;
}

BOOST_AUTO_TEST_SUITE_END()