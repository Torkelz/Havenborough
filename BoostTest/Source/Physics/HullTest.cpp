#include <boost\test\unit_test.hpp>
#include "..\..\Physics\include\Hull.h"

BOOST_AUTO_TEST_SUITE(TriangleHullTest)

	BOOST_AUTO_TEST_CASE(FindClosestPoint)
	{
		std::vector<Triangle> triangles;
		float size = 1.f;
		triangles.push_back(Triangle(Vector4( -size,  -size, -size, 1.f), Vector4(-size, size, -size, 1.f), Vector4(size,	size, -size, 1.f)));
		triangles.push_back(Triangle(Vector4( -size,  -size, -size, 1.f), Vector4( size, size, -size, 1.f), Vector4(size, -size, -size, 1.f)));

		Hull h  = Hull(triangles);
		
		DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(0.f, 5.f, 0.f);
		DirectX::XMFLOAT4X4 mtrans;
		DirectX::XMStoreFloat4x4(&mtrans, trans);
		h.updatePosition(mtrans);

		DirectX::XMFLOAT4 result = h.findClosestPointOnTriangle(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), 0);
		Triangle tri = h.getTriangleInWorldCoord(0);
		BOOST_CHECK_EQUAL(-1.f, result.x);
		BOOST_CHECK_EQUAL(4.f, result.y);
		BOOST_CHECK_EQUAL(-1.f, result.z);

		result = h.findClosestPointOnTriangle(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), 1);

		BOOST_CHECK_EQUAL(0.f, result.x);
		BOOST_CHECK_EQUAL(4.f, result.y);
		BOOST_CHECK_EQUAL(-1.f, result.z);
	}

	BOOST_AUTO_TEST_CASE(ScaleHullTest)
	{
		std::vector<Triangle> triangles;
		float size = 1.f;
		triangles.push_back(Triangle(Vector4( -size,  -size, -size, 1.f), Vector4(-size, size, -size, 1.f), Vector4(size,	size, -size, 1.f)));
		triangles.push_back(Triangle(Vector4( -size,  -size, -size, 1.f), Vector4( size, size, -size, 1.f), Vector4(size, -size, -size, 1.f)));

		Hull h  = Hull(triangles);
		
		DirectX::XMVECTOR s = DirectX::XMVectorSet(5.f, 5.f, 5.f, 0.f);
		DirectX::XMFLOAT4 sc;
		DirectX::XMStoreFloat4(&sc,s);
		h.scale(s);
		DirectX::XMFLOAT4 scale = h.getScale();
		
		BOOST_CHECK_EQUAL(sc.x, scale.x);
		BOOST_CHECK_EQUAL(sc.y, scale.y);
		BOOST_CHECK_EQUAL(sc.z, scale.z);
		BOOST_CHECK_EQUAL(sc.w, scale.w);
	}




BOOST_AUTO_TEST_SUITE_END()