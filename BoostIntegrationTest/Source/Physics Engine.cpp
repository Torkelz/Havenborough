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

std::vector<Triangle> createTriangles(float p_Size)
{
	 std::vector<Triangle> triangles;
	 float size = p_Size;
     triangles.push_back(Triangle(Vector4( -size, -size, -size, 1.f), Vector4(-size, size, -size, 1.f), Vector4(size,  size, -size, 1.f)));
     triangles.push_back(Triangle(Vector4( -size, -size, -size, 1.f), Vector4( size, size, -size, 1.f), Vector4(size, -size, -size, 1.f)));
     triangles.push_back(Triangle(Vector4( -size, size, -size, 1.f), Vector4(-size, size, size, 1.f), Vector4( size, size, size, 1.f)));
     triangles.push_back(Triangle(Vector4( -size, size, -size, 1.f), Vector4( size, size, size, 1.f), Vector4( size, size, -size, 1.f)));
     triangles.push_back(Triangle(Vector4( -size, -size, size, 1.f), Vector4( size, size, size, 1.f), Vector4(-size, size, size, 1.f)));
     triangles.push_back(Triangle(Vector4( -size, -size, size, 1.f), Vector4( size, -size, size, 1.f), Vector4(size, size, size, 1.f)));                                                                                                                                        
     triangles.push_back(Triangle(Vector4(-size, -size, size, 1.f), Vector4( -size, size, size, 1.f), Vector4(-size, size, -size, 1.f)));
     triangles.push_back(Triangle(Vector4(-size, -size, size, 1.f), Vector4( -size, size, -size, 1.f), Vector4(-size, -size, -size, 1.f)));
     triangles.push_back(Triangle(Vector4(size, -size, -size, 1.f), Vector4(size, size, -size, 1.f), Vector4( size, size, size, 1.f)));
     triangles.push_back(Triangle(Vector4( size, -size, -size, 1.f), Vector4( size, size, size, 1.f), Vector4(size, -size, size, 1.f)));
     triangles.push_back(Triangle(Vector4( size, -size, size, 1.f), Vector4( -size, -size, size, 1.f), Vector4( -size, -size, -size, 1.f)));
     triangles.push_back(Triangle(Vector4( size, -size, -size, 1.f), Vector4( size, -size, size, 1.f), Vector4( -size, -size, -size, 1.f)));

	 return triangles;
}


BOOST_AUTO_TEST_SUITE(PhysicsEngine)
	static std::string testId = "3> ";


#pragma region // ## Step 1 ## //

BOOST_AUTO_TEST_CASE(AABBvsAABB)
{
	BOOST_MESSAGE("");
	BOOST_MESSAGE(testId + "Physics integration test starting...");
	//Hit
	AABB aabb1 = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	AABB aabb2 = AABB(DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	BOOST_MESSAGE(testId + "Testing AABB vs AABB collision");
	HitData hit = Collision::boundingVolumeVsBoundingVolume(&aabb1, &aabb2);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::AABBVSAABB);
	//Miss
	aabb2 = AABB(DirectX::XMFLOAT4(3.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	hit = Collision::boundingVolumeVsBoundingVolume(&aabb1, &aabb2);
	BOOST_CHECK(!hit.intersect);
	BOOST_CHECK(hit.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(OBBvsAABB)
{
	BOOST_MESSAGE(testId + "Testing AABB vs OBB collision");
	//Hit
	AABB aabb1 = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	OBB obb1 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 1.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	HitData hit = Collision::boundingVolumeVsBoundingVolume(&obb1, &aabb1);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::OBBVSAABB);
	BOOST_CHECK_SMALL(hit.colNorm.x, 0.0001f);
	BOOST_CHECK_SMALL(hit.colNorm.y, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.z, 1.f, 0.001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colLength, 100.f, 0.001f);

	//Rotated Hit
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(3.141592f/4.f);
	obb1.setRotation(rot);
	hit = Collision::boundingVolumeVsBoundingVolume(&obb1, &aabb1);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::OBBVSAABB);
	BOOST_CHECK_SMALL(hit.colNorm.x, 0.0001f);
	BOOST_CHECK_SMALL(hit.colNorm.y, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.z, 1.f, 0.001f);
	//Miss
	obb1.setPosition(DirectX::XMVectorSet(0.f, 0.f, 3.f, 1.f));
	hit = Collision::boundingVolumeVsBoundingVolume(&obb1, &aabb1);
	BOOST_CHECK(!hit.intersect);
	BOOST_CHECK(hit.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(AABBvsSphere)
{
	BOOST_MESSAGE(testId + "Testing AABB vs Sphere collision");
	//Hit
	Sphere s1 = Sphere(5.f, DirectX::XMFLOAT4(0.f, 5.f, 0.f, 1.f));
	AABB aabb1 = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	HitData hit = Collision::boundingVolumeVsBoundingVolume(&aabb1, &s1);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::AABBVSSPHERE);
	BOOST_CHECK_SMALL(hit.colNorm.x, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.y, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hit.colNorm.z, 0.0001f);
	BOOST_CHECK_EQUAL(hit.colLength, 100.f);
	//Miss
	s1 = Sphere(5.f, DirectX::XMFLOAT4(0.f, 7.f, 0.f, 1.f));
	hit = Collision::boundingVolumeVsBoundingVolume(&aabb1, &s1);
	BOOST_CHECK(!hit.intersect);
	BOOST_CHECK(hit.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(SpherevsSphere)
{
	BOOST_MESSAGE(testId + "Testing sphere vs sphere collision");
	//Hit
	Sphere s1 = Sphere(10.f, DirectX::XMFLOAT4( 5.f, 0.f, 0.f, 1.f));
	Sphere s2 = Sphere(10.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	HitData hit = Collision::boundingVolumeVsBoundingVolume(&s1, &s2);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::SPHEREVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.x, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hit.colNorm.y, 0.0001f);
	BOOST_CHECK_SMALL(hit.colNorm.z, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colLength, 1500.f, 0.001f);
	//Miss
	s1 = Sphere(1.f, DirectX::XMFLOAT4( 5.f, 0.f, 0.f, 1.f));
	s2 = Sphere(1.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	hit = Collision::boundingVolumeVsBoundingVolume(&s1, &s2);
	BOOST_CHECK(!hit.intersect);
	BOOST_CHECK(hit.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(SpherevsOBB)
{
	BOOST_MESSAGE(testId + "Testing sphere vs OBB collision");
	//Hit
	OBB obb1 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(5.f, 5.f, 5.f, 0.f));
	Sphere s1 = Sphere(5.f, DirectX::XMFLOAT4( 5.f, 0.f, 0.f, 1.f));
	HitData hit = Collision::boundingVolumeVsBoundingVolume(&obb1, &s1);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::OBBVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.x, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hit.colNorm.y, 0.0001f);
	BOOST_CHECK_SMALL(hit.colNorm.z, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colLength, 500.f, 0.001f);
	//Miss
	obb1.setPosition(DirectX::XMVectorSet(-6.f, 0.f, 0.f, 1.f));
	hit = Collision::boundingVolumeVsBoundingVolume(&obb1, &s1);
	BOOST_CHECK(!hit.intersect);
	BOOST_CHECK(hit.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(SpherevsHull)
{
	BOOST_MESSAGE(testId + "Testing sphere vs Hull collision");


	 std::vector<Triangle> t = createTriangles(5.0f);
     Hull hull = Hull(t);
	 Sphere s1 = Sphere(1.f, DirectX::XMFLOAT4( 5.f, 0.f, 0.f, 1.f));
	 //Hit
	 HitData hit = Collision::boundingVolumeVsBoundingVolume(&hull, &s1);
	 BOOST_CHECK(hit.intersect);
	 BOOST_CHECK(hit.colType == Type::HULLVSSPHERE);
	 BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.x, 1.f, 0.001f);
	 BOOST_CHECK_SMALL(hit.colNorm.y, 0.0001f);
	 BOOST_CHECK_SMALL(hit.colNorm.z, 0.0001f);
	 BOOST_CHECK_CLOSE_FRACTION(hit.colLength, 100.f, 0.001f);
	 //Miss
	 hull.scale(DirectX::XMVectorSet(0.2f, 0.2f, 0.2f, 0.f));
	 hit = Collision::boundingVolumeVsBoundingVolume(&hull, &s1);
	 BOOST_CHECK(!hit.intersect);
	 BOOST_CHECK(hit.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(OBBvsOBB)
{
	BOOST_MESSAGE(testId + "Testing OBB vs OBB collision");

	OBB obb1 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 1.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	OBB obb2 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(3.141592f/4.f);
	DirectX::XMMATRIX invRot = DirectX::XMMatrixInverse(&DirectX::XMVectorSet(1.f, 1.f, 1.f, 1.f), rot);
	obb1.setRotation(rot);
	obb2.setRotation(invRot);
	//Hit
	HitData hit = Collision::boundingVolumeVsBoundingVolume(&obb1, &obb2);
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::OBBVSOBB);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.x, 0.7f, 0.1f);
	BOOST_CHECK_SMALL(hit.colNorm.y, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.z, 0.7f, 0.1f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colLength, 130.f, 0.1f);
	//Miss
	obb1.setRotation(invRot);
	obb2.setRotation(rot);
	obb1.setPosition(DirectX::XMVectorSet(0.f, 0.f, 3.f, 1.f));
	hit = Collision::boundingVolumeVsBoundingVolume(&obb1, &obb2);
	BOOST_CHECK(!hit.intersect);
	BOOST_CHECK(hit.colType == Type::NONE);
}
#pragma endregion


#pragma region // ## Step 2 ## //
BOOST_AUTO_TEST_CASE(BodyTestStart)
{
	BOOST_MESSAGE(testId + "Creating bodies...");
	Sphere *s = new Sphere(1.f ,DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	AABB *a = new AABB(DirectX::XMFLOAT4(5.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	OBB *b = new OBB(DirectX::XMFLOAT4(0.f, 5.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	OBB *b2 = new OBB(DirectX::XMFLOAT4(0.f, 0.f, 5.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	std::vector<Triangle> t = createTriangles(1.f);
	Hull *h = new Hull(t);

	std::vector<Body> bodies;
	bodies.push_back(Body(50.f, std::unique_ptr<BoundingVolume> (s), false, false));
	bodies.push_back(Body(50.f, std::unique_ptr<BoundingVolume> (a), true, false));
	bodies.push_back(Body(50.f, std::unique_ptr<BoundingVolume>(b), true, false));
	bodies.push_back(Body(50.f, std::unique_ptr<BoundingVolume>(b2), false, false));
	bodies.push_back(Body(50.f, std::unique_ptr<BoundingVolume>(h), true, false));
	bodies.at(4).setPosition(DirectX::XMFLOAT4(-5.f, 0.f, 0.f, 0.f));


	BOOST_MESSAGE(testId + "Testing collision between bodies");
	HitData hit = HitData();
	for(unsigned int i = 1; i < bodies.size(); i++)
	{
		hit = Collision::boundingVolumeVsBoundingVolume(bodies.at(0).getVolume(), bodies.at(i).getVolume());
		BOOST_CHECK(!hit.intersect);
		BOOST_CHECK(hit.colType == Type::NONE);
	}
	for(unsigned int i = 0; i < bodies.size(); i++)
	{
		if(i == 3)
			continue;
		hit = Collision::boundingVolumeVsBoundingVolume(bodies.at(3).getVolume(), bodies.at(i).getVolume());
		BOOST_CHECK(!hit.intersect);
		BOOST_CHECK(hit.colType == Type::NONE);
	}


	bodies.at(0).setPosition(DirectX::XMFLOAT4(3.f, 0.f, 0.f, 1.f));
	hit = Collision::boundingVolumeVsBoundingVolume(bodies.at(0).getVolume(), bodies.at(1).getVolume());
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::AABBVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.x, -1.f, 0.001f);
	BOOST_CHECK_SMALL(hit.colNorm.y, 0.001f);
	BOOST_CHECK_SMALL(hit.colNorm.z, 0.001f);

	bodies.at(3).setPosition(DirectX::XMFLOAT4(0.f, 3.f, 0.f, 1.f));
	hit = Collision::boundingVolumeVsBoundingVolume(bodies.at(3).getVolume(), bodies.at(2).getVolume());
	BOOST_CHECK(hit.intersect);
	BOOST_CHECK(hit.colType == Type::OBBVSOBB);
	BOOST_CHECK_SMALL(hit.colNorm.x, 0.001f);
	BOOST_CHECK_CLOSE_FRACTION(hit.colNorm.y, -1.f, 0.001f);
	BOOST_CHECK_SMALL(hit.colNorm.z, 0.001f);


	BOOST_MESSAGE("");
}
#pragma endregion

#pragma region // ## Step 3 ## //

#pragma endregion

#pragma region // ## Step 4 ## //

#pragma endregion
BOOST_AUTO_TEST_SUITE_END()
