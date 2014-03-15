#include <boost\test\unit_test.hpp>
#include "..\..\Physics\Source\Octree.h"
#include "..\..\Physics\include\Sphere.h"

#include <iterator>
#include <set>

using namespace DirectX;

BOOST_AUTO_TEST_SUITE(TestOctree)

BOOST_AUTO_TEST_CASE(TestSetupOctreeByFilling)
{
	static const size_t fillNum = 5;
	Octree tree;
	Sphere spheres[fillNum];
	for (size_t i = 0; i < fillNum; ++i)
	{
		spheres[i] = Sphere(10.f, XMFLOAT4(0.f, 0.f, 0.f, 1.f));
		BOOST_CHECK_NO_THROW(tree.addBody(0, &spheres[i]));
	}

	const XMFLOAT4& min = tree.getMinPos();
	const XMFLOAT4& max = tree.getMaxPos();

	BOOST_CHECK_EQUAL(min.x, -10.f);
	BOOST_CHECK_EQUAL(min.y, -10.f);
	BOOST_CHECK_EQUAL(min.z, -10.f);
	BOOST_CHECK_EQUAL(min.w, 1.f);
	BOOST_CHECK_EQUAL(max.x, 10.f);
	BOOST_CHECK_EQUAL(max.y, 10.f);
	BOOST_CHECK_EQUAL(max.z, 10.f);
	BOOST_CHECK_EQUAL(max.w, 1.f);
}

BOOST_AUTO_TEST_CASE(TestExpandOctree)
{
	Octree tree;
	
	Sphere sphere(10.f, XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	tree.addBody(0, &sphere);

	const XMFLOAT4& min = tree.getMinPos();
	const XMFLOAT4& max = tree.getMaxPos();

	BOOST_CHECK_EQUAL(min.x, -10.f);
	BOOST_CHECK_EQUAL(min.y, -10.f);
	BOOST_CHECK_EQUAL(min.z, -10.f);
	BOOST_CHECK_EQUAL(min.w, 1.f);
	BOOST_CHECK_EQUAL(max.x, 10.f);
	BOOST_CHECK_EQUAL(max.y, 10.f);
	BOOST_CHECK_EQUAL(max.z, 10.f);
	BOOST_CHECK_EQUAL(max.w, 1.f);

	Sphere sphere2(10.f, XMFLOAT4(5.f, 0.f, 0.f, 1.f));
	tree.addBody(1, &sphere2);

	const XMFLOAT4& min2 = tree.getMinPos();
	const XMFLOAT4& max2 = tree.getMaxPos();

	BOOST_CHECK_EQUAL(min2.x, -10.f);
	BOOST_CHECK_EQUAL(min2.y, -30.f);
	BOOST_CHECK_EQUAL(min2.z, -30.f);
	BOOST_CHECK_EQUAL(min2.w, 1.f);
	BOOST_CHECK_EQUAL(max2.x, 30.f);
	BOOST_CHECK_EQUAL(max2.y, 10.f);
	BOOST_CHECK_EQUAL(max2.z, 10.f);
	BOOST_CHECK_EQUAL(max2.w, 1.f);
}

BOOST_AUTO_TEST_CASE(TestOctreeRemoveBody)
{
	Octree tree;

	BOOST_CHECK_EQUAL(tree.getBodyCount(), 0);

	Sphere sphere(10.f, XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	Sphere sphere2(5.f, XMFLOAT4(20.f, 0.f, 0.f, 1.f));

	tree.addBody(0, &sphere);
	BOOST_CHECK_EQUAL(tree.getBodyCount(), 1);

	tree.removeBody(0, &sphere);
	BOOST_CHECK_EQUAL(tree.getBodyCount(), 0);

	tree.addBody(1, &sphere);
	tree.addBody(2, &sphere2);
	BOOST_CHECK_EQUAL(tree.getBodyCount(), 2);

	tree.removeBody(2, &sphere2);
	BOOST_CHECK_EQUAL(tree.getBodyCount(), 1);
}

BOOST_AUTO_TEST_CASE(TestOctreeFind)
{
	Octree tree;

	Sphere sphere(10.f, XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	Sphere sphere2(5.f, XMFLOAT4(20.f, 0.f, 0.f, 1.f));

	tree.addBody(0, &sphere);
	tree.addBody(1, &sphere2);

	std::set<Octree::BodyHandle> potentialColliders;
	tree.findPotentialIntersections(&sphere2, std::inserter(potentialColliders, potentialColliders.end()));

	BOOST_CHECK_EQUAL(potentialColliders.size(), 1);
	BOOST_CHECK(potentialColliders.find(0) == potentialColliders.end());
	BOOST_CHECK(potentialColliders.find(1) != potentialColliders.end());
}

BOOST_AUTO_TEST_SUITE_END()
