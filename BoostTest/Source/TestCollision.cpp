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

BOOST_AUTO_TEST_CASE(AABBVsSphereHit)
{
	Collision col;
	Sphere s1;
	AABB aabb;

	s1 = Sphere(1.f, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	aabb = AABB(DirectX::XMFLOAT4(-1.f, -1.f, -1.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));

	BOOST_CHECK_EQUAL(col.AABBvsSphere(&aabb, &s1).intersect, true);
}

BOOST_AUTO_TEST_CASE(EXTREMECASE_AABBVsSphereHit)
{
	Collision col;
	Sphere s1;
	AABB aabb;

	s1 = Sphere(0.1f, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	aabb = AABB(DirectX::XMFLOAT4(-100.f, -100.f, -100.f, 1.f), DirectX::XMFLOAT4(100.f, 100.f, 100.f, 1.f));

	BOOST_CHECK_EQUAL(col.AABBvsSphere(&aabb, &s1).intersect, true);
}

BOOST_AUTO_TEST_CASE(EXTREMECASE2_AABBVsSphereHit)
{
	Collision col;
	Sphere s1;
	AABB aabb;

	s1 = Sphere(1000.f, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	aabb = AABB(DirectX::XMFLOAT4(-1.f, -1.f, -1.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));

	BOOST_CHECK_EQUAL(col.AABBvsSphere(&aabb, &s1).intersect, true);
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

	aabb = AABB(DirectX::XMFLOAT4(-1.f, -1.f, -1.f, 1.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	aabbb = AABB(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f), DirectX::XMFLOAT4(2.f, 2.f, 2.f, 1.f));

	BOOST_CHECK_EQUAL(col.AABBvsAABB(&aabb, &aabbb).intersect, false);
}

BOOST_AUTO_TEST_SUITE_END()