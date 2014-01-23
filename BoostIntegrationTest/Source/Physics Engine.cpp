#include <boost/test/unit_test.hpp>
#include "IPhysics.h"
#include "VolumeIncludeAll.h"
#include "PhysicsTypes.h"
#include "../../Physics/Source/Collision.h"
#include "../../Physics/Source/Body.h"
#include "../../Physics/Source/BVLoader.h"
#include "../../Physics/Source/PhysicsLogger.h"

#if _DEBUG
#include <vld.h>
#endif

BOOST_AUTO_TEST_SUITE(PhysicsEngine)
	static std::string testId = "3> ";


BOOST_AUTO_TEST_CASE(PhysicsTest1)
{
	BOOST_MESSAGE(testId + "Testing Physics...");

	// ## Step 1 ## //
	//Hit
	AABB aabb1 = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	AABB aabb2 = AABB(DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	BOOST_MESSAGE(testId + "Testing AABB vs AABB collision");
	HitData hit = Collision::AABBvsAABB(&aabb1, &aabb2);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::AABBVSAABB);
	//Miss
	aabb2 = AABB(DirectX::XMFLOAT4(3.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	hit = Collision::AABBvsAABB(&aabb1, &aabb2);
	BOOST_CHECK(!hit.intersect);
	BOOST_CHECK(hit.colType == Type::NONE);

	BOOST_MESSAGE(testId + "Testing AABB vs Sphere collision");
	//Hit
	Sphere s1 = Sphere(5.f, DirectX::XMFLOAT4(0.f, 5.f, 0.f, 1.f));
	hit = Collision::AABBvsSphere(&aabb1, &s1);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::AABBVSSPHERE);
	BOOST_CHECK_SMALL(hit.colNorm.x, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.y, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hit.colNorm.z, 0.0001f);
	BOOST_CHECK_EQUAL(hit.colLength, 100.f);
	//Miss
	s1 = Sphere(5.f, DirectX::XMFLOAT4(0.f, 7.f, 0.f, 1.f));
	hit = Collision::AABBvsSphere(&aabb1, &s1);
	BOOST_CHECK(!hit.intersect);
	BOOST_CHECK(hit.colType == Type::NONE);

	BOOST_MESSAGE(testId + "Testing AABB vs OBB collision");
	//Hit
	OBB obb1 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 1.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	hit = Collision::OBBvsAABB(&obb1, &aabb1);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::OBBVSAABB);
	BOOST_CHECK_SMALL(hit.colNorm.x, 0.0001f);
	BOOST_CHECK_SMALL(hit.colNorm.y, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.z, 1.f, 0.001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colLength, 100.f, 0.001f);
	//Rotated Hit
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(3.141592f/4.f);
	obb1.setRotation(rot);
	hit = Collision::OBBvsAABB(&obb1, &aabb1);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::OBBVSAABB);
	BOOST_CHECK_SMALL(hit.colNorm.x, 0.0001f);
	BOOST_CHECK_SMALL(hit.colNorm.y, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.z, 1.f, 0.001f);
	//Miss
	obb1.setPosition(DirectX::XMVectorSet(0.f, 0.f, 3.f, 1.f));
	hit = Collision::OBBvsAABB(&obb1, &aabb1);
	BOOST_CHECK(!hit.intersect);
	BOOST_CHECK(hit.colType == Type::NONE);

	BOOST_MESSAGE(testId + "Testing sphere vs sphere collision");
	//Hit
	s1 = Sphere(10.f, DirectX::XMFLOAT4( 5.f, 0.f, 0.f, 1.f));
	Sphere s2 = Sphere(10.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	hit = Collision::sphereVsSphere(&s1, &s2);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::SPHEREVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.x, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hit.colNorm.y, 0.0001f);
	BOOST_CHECK_SMALL(hit.colNorm.z, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colLength, 1500.f, 0.001f);
	//Miss
	s1 = Sphere(1.f, DirectX::XMFLOAT4( 5.f, 0.f, 0.f, 1.f));
	s2 = Sphere(1.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	hit = Collision::sphereVsSphere(&s1, &s2);
	BOOST_CHECK(!hit.intersect);
	BOOST_CHECK(hit.colType == Type::NONE);

	BOOST_MESSAGE(testId + "Testing sphere vs OBB collision");

	BOOST_MESSAGE(testId + "Testing sphere vs Hull collision");

	BOOST_MESSAGE(testId + "Testing OBB vs OBB collision");
}

BOOST_AUTO_TEST_SUITE_END()