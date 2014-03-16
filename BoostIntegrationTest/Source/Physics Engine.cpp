#include <boost/test/unit_test.hpp>
#include "IPhysics.h"
#include "VolumeIncludeAll.h"
#include "PhysicsTypes.h"
#include "../../Physics/Source/Collision.h"
#include "../../Physics/Source/Body.h"
#include "../../Physics/Source/BVLoader.h"
#include "../../Physics/Source/PhysicsLogger.h"
#include "../../Common/Source/ResourceManager.h"

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
	static float dt = 0.137338176f;


#pragma region // ## Step 1 ## //

BOOST_AUTO_TEST_CASE(AABBvsAABBIntegration)
{
	BOOST_MESSAGE("");
	BOOST_MESSAGE(testId + "Physics integration test starting...");
	//Hit
	AABB aabb1 = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	AABB aabb2 = AABB(DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	BOOST_MESSAGE(testId + "Testing AABB vs AABB collision");
	HitData hd = Collision::boundingVolumeVsBoundingVolume(aabb1, aabb2);
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::AABBVSAABB);
	//Miss
	aabb2 = AABB(DirectX::XMFLOAT4(3.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	hd = Collision::boundingVolumeVsBoundingVolume(aabb1, aabb2);
	BOOST_CHECK(!hd.intersect);
	BOOST_CHECK(hd.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(OBBvsAABBIntegration)
{
	BOOST_MESSAGE(testId + "Testing AABB vs OBB collision");
	//Hit
	AABB aabb1 = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	OBB obb1 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 1.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	HitData hd = Collision::boundingVolumeVsBoundingVolume(obb1, aabb1);
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::OBBVSAABB);
	BOOST_CHECK_SMALL(hd.colNorm.x, 0.0001f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.z, 1.f, 0.001f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colLength, 100.f, 0.001f);

	//Rotated Hit
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(3.141592f/4.f);
	obb1.setRotation(rot);
	hd = Collision::boundingVolumeVsBoundingVolume(obb1, aabb1);
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::OBBVSAABB);
	BOOST_CHECK_SMALL(hd.colNorm.x, 0.0001f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.z, 1.f, 0.001f);
	//Miss
	obb1.setPosition(DirectX::XMVectorSet(0.f, 0.f, 3.f, 1.f));
	hd = Collision::boundingVolumeVsBoundingVolume(obb1, aabb1);
	BOOST_CHECK(!hd.intersect);
	BOOST_CHECK(hd.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(AABBvsSphereIntegration)
{
	BOOST_MESSAGE(testId + "Testing AABB vs Sphere collision");
	//Hit
	Sphere s1 = Sphere(5.f, DirectX::XMFLOAT4(0.f, 5.f, 0.f, 1.f));
	AABB aabb1 = AABB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	HitData hd = Collision::boundingVolumeVsBoundingVolume(aabb1, s1);
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::AABBVSSPHERE);
	BOOST_CHECK_SMALL(hd.colNorm.x, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.y, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.z, 0.0001f);
	BOOST_CHECK_EQUAL(hd.colLength, 100.f);
	//Miss
	s1 = Sphere(5.f, DirectX::XMFLOAT4(0.f, 7.f, 0.f, 1.f));
	hd = Collision::boundingVolumeVsBoundingVolume(aabb1, s1);
	BOOST_CHECK(!hd.intersect);
	BOOST_CHECK(hd.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(SpherevsSphereIntegration)
{
	BOOST_MESSAGE(testId + "Testing sphere vs sphere collision");
	//Hit
	Sphere s1 = Sphere(10.f, DirectX::XMFLOAT4( 5.f, 0.f, 0.f, 1.f));
	Sphere s2 = Sphere(10.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	HitData hd = Collision::boundingVolumeVsBoundingVolume(s1, s2);
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::SPHEREVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.x, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.0001f);
	BOOST_CHECK_SMALL(hd.colNorm.z, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colLength, 1500.f, 0.001f);
	//Miss
	s1 = Sphere(1.f, DirectX::XMFLOAT4( 5.f, 0.f, 0.f, 1.f));
	s2 = Sphere(1.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	hd = Collision::boundingVolumeVsBoundingVolume(s1, s2);
	BOOST_CHECK(!hd.intersect);
	BOOST_CHECK(hd.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(SpherevsOBBIntegration)
{
	BOOST_MESSAGE(testId + "Testing sphere vs OBB collision");
	//Hit
	OBB obb1 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(5.f, 5.f, 5.f, 0.f));
	Sphere s1 = Sphere(5.f, DirectX::XMFLOAT4( 5.f, 0.f, 0.f, 1.f));
	HitData hd = Collision::boundingVolumeVsBoundingVolume(obb1, s1);
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::OBBVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.x, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.0001f);
	BOOST_CHECK_SMALL(hd.colNorm.z, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colLength, 500.f, 0.001f);
	//Miss
	obb1.setPosition(DirectX::XMVectorSet(-6.f, 0.f, 0.f, 1.f));
	hd = Collision::boundingVolumeVsBoundingVolume(obb1, s1);
	BOOST_CHECK(!hd.intersect);
	BOOST_CHECK(hd.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(SpherevsHullIntegration)
{
	BOOST_MESSAGE(testId + "Testing sphere vs Hull collision");


	 std::vector<Triangle> t = createTriangles(5.0f);
     Hull hull = Hull(t);
	 Sphere s1 = Sphere(1.f, DirectX::XMFLOAT4( 5.f, 0.f, 0.f, 1.f));
	 //Hit
	 HitData hd = Collision::boundingVolumeVsBoundingVolume(hull, s1);
	 BOOST_CHECK(hd.intersect);
	 BOOST_CHECK(hd.colType == Type::HULLVSSPHERE);
	 BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.x, 1.f, 0.001f);
	 BOOST_CHECK_SMALL(hd.colNorm.y, 0.0001f);
	 BOOST_CHECK_SMALL(hd.colNorm.z, 0.0001f);
	 BOOST_CHECK_CLOSE_FRACTION(hd.colLength, 100.f, 0.001f);
	 //Miss
	 hull.scale(DirectX::XMVectorSet(0.2f, 0.2f, 0.2f, 0.f));
	 hd = Collision::boundingVolumeVsBoundingVolume(hull, s1);
	 BOOST_CHECK(!hd.intersect);
	 BOOST_CHECK(hd.colType == Type::NONE);
}

BOOST_AUTO_TEST_CASE(OBBvsOBBIntegration)
{
	BOOST_MESSAGE(testId + "Testing OBB vs OBB collision");

	OBB obb1 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 1.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	OBB obb2 = OBB(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(3.141592f/4.f);
	DirectX::XMMATRIX invRot = DirectX::XMMatrixInverse(&DirectX::XMVectorSet(1.f, 1.f, 1.f, 1.f), rot);
	obb1.setRotation(rot);
	obb2.setRotation(invRot);
	//Hit
	HitData hd = Collision::boundingVolumeVsBoundingVolume(obb1, obb2);
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::OBBVSOBB);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.x, 0.7f, 0.1f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.z, 0.7f, 0.1f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colLength, 130.f, 0.1f);
	//Miss
	obb1.setRotation(invRot);
	obb2.setRotation(rot);
	obb1.setPosition(DirectX::XMVectorSet(0.f, 0.f, 3.f, 1.f));
	hd = Collision::boundingVolumeVsBoundingVolume(obb1, obb2);
	BOOST_CHECK(!hd.intersect);
	BOOST_CHECK(hd.colType == Type::NONE);
}
#pragma endregion

#pragma region // ## Step 2 ## //
BOOST_AUTO_TEST_CASE(BodyIntegration)
{
	BOOST_MESSAGE(testId + "Creating bodies...");

	std::vector<Body> bodies;
	//Creates 3 stationary objects and 2 movable.
	bodies.push_back(Body(50.f, std::unique_ptr<BoundingVolume>(new Sphere(1.f ,DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f))), false, false));
	bodies.push_back(Body(50.f, std::unique_ptr<BoundingVolume>(new AABB(DirectX::XMFLOAT4(5.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f))), true, false));
	bodies.push_back(Body(50.f, std::unique_ptr<BoundingVolume>(new OBB(DirectX::XMFLOAT4(0.f, 5.f, 0.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f))), true, false));
	bodies.push_back(Body(50.f, std::unique_ptr<BoundingVolume>(new OBB(DirectX::XMFLOAT4(0.f, 0.f, 5.f, 1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f))), false, false));
	std::vector<Triangle> t = createTriangles(1.f);
	bodies.push_back(Body(50.f, std::unique_ptr<BoundingVolume>(new Hull(t)), true, false));
	bodies.at(4).setPosition(DirectX::XMFLOAT4(-5.f, 0.f, 0.f, 0.f));

	BOOST_MESSAGE(testId + "Testing collision between bodies");
	//Check so no bodies are colliding at the start.
	HitData hd = HitData();
	for(unsigned int i = 1; i < bodies.size(); i++)
	{
		hd = Collision::boundingVolumeVsBoundingVolume(*bodies.at(0).getVolume(), *bodies.at(i).getVolume());
		BOOST_CHECK(!hd.intersect);
		BOOST_CHECK(hd.colType == Type::NONE);
	}
	for(unsigned int i = 0; i < bodies.size(); i++)
	{
		if(i == 3)
			continue;
		hd = Collision::boundingVolumeVsBoundingVolume(*bodies.at(3).getVolume(), *bodies.at(i).getVolume());
		BOOST_CHECK(!hd.intersect);
		BOOST_CHECK(hd.colType == Type::NONE);
	}

	//Collide movable sphere with static AABB
	bodies.at(0).setPosition(DirectX::XMFLOAT4(3.f, 0.f, 0.f, 1.f));
	hd = Collision::boundingVolumeVsBoundingVolume(*bodies.at(0).getVolume(), *bodies.at(1).getVolume());
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::AABBVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.x, -1.f, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.z, 0.001f);

	//Collide movable OBB with static OBB
	bodies.at(3).setPosition(DirectX::XMFLOAT4(0.f, 3.f, 0.f, 1.f));
	hd = Collision::boundingVolumeVsBoundingVolume(*bodies.at(3).getVolume(), *bodies.at(2).getVolume());
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::OBBVSOBB);
	BOOST_CHECK_SMALL(hd.colNorm.x, 0.001f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.y, -1.f, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.z, 0.001f);

	//Collide movable sphere with static OBB
	bodies.at(0).setPosition(DirectX::XMFLOAT4(1.f, 5.f, 0.f, 1.f));
	hd = Collision::boundingVolumeVsBoundingVolume(*bodies.at(0).getVolume(), *bodies.at(2).getVolume());
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::OBBVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.x, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.z, 0.001f);

	//Collide OBB  with static AABB
	bodies.at(3).setPosition(DirectX::XMFLOAT4(3.f, 0.f, 2.f, 1.f));
	hd = Collision::boundingVolumeVsBoundingVolume(*bodies.at(3).getVolume(), *bodies.at(1).getVolume());
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::OBBVSAABB);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.x, -1.f, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.z, 0.001f);

	//Collide the two moving bodies by giving them a force pushing them into eachother
	bodies.at(0).setPosition(DirectX::XMFLOAT4(3.f, 5.f, 2.f, 1.f));
	bodies.at(0).addForce(DirectX::XMFLOAT4(0.f, -50.f, 0.f, 0.f));
	bodies.at(3).addForce(DirectX::XMFLOAT4(0.f, 50.f, 0.f, 0.f));
	for(int i = 0; i < 20; i++)
	{
		bodies.at(0).update(dt);
		bodies.at(3).update(dt);
		hd = Collision::boundingVolumeVsBoundingVolume(*bodies.at(0).getVolume(), *bodies.at(3).getVolume());
		if(hd.intersect)
			break;
	}
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::OBBVSSPHERE);
	BOOST_CHECK_SMALL(hd.colNorm.x, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.y, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.z, 0.0001f);

	//Collide movable sphere with static hull.
	bodies.at(0).setPosition(DirectX::XMFLOAT4(-4.f, 0.f, 1.f, 1.f));
	hd = Collision::boundingVolumeVsBoundingVolume(*bodies.at(0).getVolume(), *bodies.at(4).getVolume());
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::HULLVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.z, 0.7f, 0.1f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.0001f);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.x, 0.7f, 0.1f);

	Body::resetBodyHandleCounter();
}
#pragma endregion

#pragma region // ## Step 3 ## //
BOOST_AUTO_TEST_CASE(BVLoaderIntegration)
{
	BOOST_MESSAGE(testId + "Testing to create bounding volumes from file");
	BVLoader bvl;
	BOOST_MESSAGE(testId + "Loading bounding volume from file");
	BOOST_CHECK(!bvl.loadBinaryFile("../Source/gun_battle.mp3"));
	BOOST_CHECK(bvl.loadBinaryFile("../Bin/assets/volumes/CB_Barrel1.txc"));
	std::vector<BVLoader::BoundingVolume> BV = bvl.getBoundingVolumes();
	
	//Create triangles for hull
	std::vector<Triangle> triangles;
	Triangle triangle;
	for(unsigned int i = 0; i < BV.size()/3; i++)
	{
		triangle.corners[0] = BV[i * 3].m_Postition;
		triangle.corners[1] = BV[i * 3 + 1].m_Postition;
		triangle.corners[2] = BV[i * 3 + 2].m_Postition;

		triangles.push_back(triangle);
	}

	//Create a hull and sphere(Collider)
	Hull *h = new Hull(triangles);
	Sphere *s = new Sphere(5.f ,DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	Body bs = Body(50.f, std::unique_ptr<BoundingVolume> (s), false, false);
	Body bh = Body(50.f, std::unique_ptr<BoundingVolume>(h), true, false);
	bs.setPosition(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	bh.setPosition(DirectX::XMFLOAT4(-20.f, 0.f, 0.f, 0.f));
	DirectX::XMVECTOR v = DirectX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.f);
	bh.getVolume()->scale(v);

	//Load another BV
	BOOST_CHECK(bvl.loadBinaryFile("../Bin/assets/volumes/CB_Crate1.txc"));
	BV = bvl.getBoundingVolumes();
	triangles.clear();
	for(unsigned int i = 0; i < BV.size()/3; i++)
	{
		triangle.corners[0] = BV[i * 3].m_Postition;
		triangle.corners[1] = BV[i * 3 + 1].m_Postition;
		triangle.corners[2] = BV[i * 3 + 2].m_Postition;

		triangles.push_back(triangle);
	}
	triangles.shrink_to_fit();
	h = new Hull(triangles);
	Body bh1 = Body(25.f, std::unique_ptr<BoundingVolume>(h), true, false);
	bh1.setPosition(DirectX::XMFLOAT4(20.f, 0.f, 0.f, 0.f));
	bh1.getVolume()->scale(v);

	BOOST_MESSAGE(testId + "Testing to collision with bounding volumes loaded from file");
	//Check so no bodies are colliding from the start
	HitData hd = HitData();
	hd = Collision::boundingVolumeVsBoundingVolume(*bs.getVolume(), *bh.getVolume());
	BOOST_CHECK(!hd.intersect);
	BOOST_CHECK(hd.colType == Type::NONE);
	hd = Collision::boundingVolumeVsBoundingVolume(*bs.getVolume(), *bh1.getVolume());
	BOOST_CHECK(!hd.intersect);
	BOOST_CHECK(hd.colType == Type::NONE);

	//move the sphere with force against the box to the left.
	bs.addForce(DirectX::XMFLOAT4(150.f, 0.f, 0.f, 0.f));
	for(int i = 0; i < 20; i++)
		bs.update(dt);

	//Collision with left box
	hd = Collision::boundingVolumeVsBoundingVolume(*bs.getVolume(), *bh1.getVolume());
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::HULLVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.x, -1.f, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.0001f);
	BOOST_CHECK_SMALL(hd.colNorm.z, 0.0001f);

	//move the sphere with GREAT force against the right box.
	bs.addForce(DirectX::XMFLOAT4(-750.f, 0.f, 0.f, 0.f));
	for(int i = 0; i < 20; i++)
		bs.update(dt);

	//Collision with right box
	hd = Collision::boundingVolumeVsBoundingVolume(*bs.getVolume(), *bh.getVolume());
	BOOST_CHECK(hd.intersect);
	BOOST_CHECK(hd.colType == Type::HULLVSSPHERE);
	BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.x, 1.f, 0.001f);
	BOOST_CHECK_SMALL(hd.colNorm.y, 0.0001f);
	BOOST_CHECK_SMALL(hd.colNorm.z, 0.0001f);
	Body::resetBodyHandleCounter();
}
#pragma endregion

#pragma region // ## Step 4 ## //
BOOST_AUTO_TEST_CASE(IPhysicsIntegration)
{
	BOOST_MESSAGE(testId + "Testing physics interface");
	IPhysics * physics;
	physics = IPhysics::createPhysics();
	physics->initialize(false, 1.f / 60.f);
	BOOST_MESSAGE(testId + "Physics engine initialized");
	BOOST_MESSAGE(testId + "Using physics engine to create bodies with different bounding volumes");
	BodyHandle aabb = physics->createAABB(40.f, true, Vector3(0.f, 0.f, 0.f), Vector3(100.f, 100.f, 100.f), false);
	BodyHandle obb = physics->createOBB(40.f, true, Vector3(0.f, 0.f, 0.f), Vector3(100.f, 100.f, 100.f), false);
	BodyHandle sphere = physics->createSphere(40.f, false, Vector3(0.f, 0.f, 0.f), 100.f);
	BOOST_MESSAGE(testId + "Using physics engine to load bounding volume from file");
	BOOST_CHECK(!physics->createBV("Gun Battle","../Source/gun_battle.mp3"));
	BOOST_CHECK(physics->createBV("Barrel1","../Bin/assets/volumes/CB_Barrel1.txc"));
	BodyHandle hull = physics->createBVInstance("Barrel1");
	BOOST_MESSAGE(testId + "Using physics engine to manipulate the created bodies");
	physics->setBodyPosition(hull, Vector3(1000.f, 0.f, 0.f));
	BOOST_CHECK_EQUAL(physics->getBodyPosition(hull).x, 1000.f);
	BOOST_CHECK_EQUAL(physics->getBodyPosition(hull).y, 0.f);
	BOOST_CHECK_EQUAL(physics->getBodyPosition(hull).z, 0.f);
	physics->setBodyPosition(aabb, Vector3(0.f, 0.f, 0.f));
	BOOST_CHECK_EQUAL(physics->getBodyPosition(aabb).x, 0.f);
	BOOST_CHECK_EQUAL(physics->getBodyPosition(aabb).y, 0.f);
	BOOST_CHECK_EQUAL(physics->getBodyPosition(aabb).z, 0.f);
	physics->setBodyPosition(obb, Vector3(0.f, 0.f, 1000.f));
	BOOST_CHECK_EQUAL(physics->getBodyPosition(obb).x, 0.f);
	BOOST_CHECK_EQUAL(physics->getBodyPosition(obb).y, 0.f);
	BOOST_CHECK_EQUAL(physics->getBodyPosition(obb).z, 1000.f);
	physics->setBodyPosition(sphere, Vector3(0.f, 500.f, 0.f));
	BOOST_CHECK_EQUAL(physics->getBodyPosition(sphere).x, 0.f);
	BOOST_CHECK_EQUAL(physics->getBodyPosition(sphere).y, 500.f);
	BOOST_CHECK_EQUAL(physics->getBodyPosition(sphere).z, 0.f);
	physics->setBodyRotation(obb, Vector3(3.14f/4.f, 0.f, 3.14f/4.f));
	physics->setBodyRotation(hull, Vector3(3.14f/4.f, 0.f, 0.f));
	physics->setBodyScale(hull, Vector3(2.f, 2.f, 2.f));
	Vector3 s = physics->getBodySize(hull);
	BOOST_CHECK_EQUAL(physics->getBodySize(hull).x, 200.f);
	BOOST_CHECK_EQUAL(physics->getBodySize(hull).y, 200.f);
	BOOST_CHECK_EQUAL(physics->getBodySize(hull).z, 200.f);


	BOOST_MESSAGE(testId + "Testing collision between static bodies and bodies that are affected by gravity");
	//Sphere falls on hull
	for(int i = 0; i < 5; i++)
	{
		physics->update(dt, 50);
		unsigned int size = physics->getHitDataSize();
		if(size > 0)
		{
			for(int j = size -1; j >= 0; j--)
			{
				HitData hd = physics->getHitDataAt(j);
				BOOST_CHECK(hd.colType == Type::AABBVSSPHERE);
				break;
			}
		}
	}

	physics->setBodyPosition(hull, Vector3(10000.f, 0.f, 0.f));
	physics->setBodyPosition(obb, Vector3(0.f, 0.f, 0.f));
	//Shoot the sphere up in the air
	physics->applyForce(sphere, Vector3(0.f, 1500.f, 0.f));
	//Sphere falls on obb
	for(int i = 0; i < 41; i++)
	{
		physics->update(dt, 50);
		unsigned int size = physics->getHitDataSize();
		if(size > 0 && i > 1)
		{
			for(int j = size -1; j >= 0; j--)
			{
				HitData hd = physics->getHitDataAt(j);
				BOOST_CHECK(hd.colType == Type::OBBVSSPHERE);
				BOOST_CHECK_SMALL(hd.colNorm.x, 0.001f);
				BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.y, -1.f, 0.0001f);
				BOOST_CHECK_SMALL(hd.colNorm.z, 0.001f);
			}
		}
	}
	physics->releaseAllBoundingVolumes();
	IPhysics::deletePhysics(physics);
	physics = nullptr;
	BOOST_CHECK(physics == nullptr);
	Body::resetBodyHandleCounter();
}
#pragma endregion

#pragma region // ## Step 5 ## //
BOOST_AUTO_TEST_CASE(PhysicsResourceIntegration)
{
	BOOST_MESSAGE(testId + "Testing to load bounding volumes through the resource manager");
	IPhysics *physics = IPhysics::createPhysics();
	physics->initialize(false, 1.f / 60.f);
	BOOST_MESSAGE(testId + "Initializing the resource manager");
	std::unique_ptr<ResourceManager> resourceManager(new ResourceManager);
	resourceManager->loadDataFromFile("assets/Resources.xml");

	using namespace std::placeholders;
	BOOST_MESSAGE(testId + "Registering create and release functions for bounding volumes");
	resourceManager->registerFunction("volume", std::bind(&IPhysics::createBV, physics, _1, _2), std::bind(&IPhysics::releaseBV, physics, _1));
	
	BOOST_MESSAGE(testId + "Loading bounding volume through the resource manager");
	int resourceID = resourceManager->loadResource("volume", "Barrel1");
	int instanceID = physics->createBVInstance("Barrel1");

	BOOST_MESSAGE(testId + "Testing collision with the loaded bounding volume");
	BodyHandle sphere = physics->createSphere(40.f, false, Vector3(0.f, 500.f, 0.f), 100.f);

	for(int i = 0; i < 41; i++)
	{
		physics->update(dt, 50);
		unsigned int size = physics->getHitDataSize();
		if(size > 0 && i > 1)
		{
			for(int j = size -1; j >= 0; j--)
			{
				HitData hd = physics->getHitDataAt(j);
				BOOST_CHECK(hd.colType == Type::HULLVSSPHERE);
				BOOST_CHECK_SMALL(hd.colNorm.x, 0.001f);
				BOOST_CHECK_CLOSE_FRACTION(hd.colNorm.y, 1.f, 0.0001f);
				BOOST_CHECK_SMALL(hd.colNorm.z, 0.001f);
			}
		}
	}


	BOOST_CHECK(resourceManager->releaseResource(resourceID));
	
	resourceManager->unregisterResourceType("volume");
	IPhysics::deletePhysics(physics);
	physics = nullptr;
	Body::resetBodyHandleCounter();
		BOOST_MESSAGE(testId + "Physics integration test completed");
	BOOST_MESSAGE("");
	BOOST_MESSAGE("");
}
#pragma endregion

BOOST_AUTO_TEST_SUITE_END()
