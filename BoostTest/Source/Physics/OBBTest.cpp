#include <boost\test\unit_test.hpp>
#include "..\..\Physics\include\OBB.h"

BOOST_AUTO_TEST_SUITE(OrientedBoundingBox)

	BOOST_AUTO_TEST_CASE(FindClosestPoint)
	{
		OBB obb = OBB(DirectX::XMFLOAT4(0.f,0.f,0.f,1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
		DirectX::XMVECTOR point = DirectX::XMVectorSet(-3.f, 0.f, 0.f, 0.f);
		DirectX::XMFLOAT4 result;
		DirectX::XMStoreFloat4(&result, obb.findClosestPt(point));

		BOOST_CHECK_EQUAL(result.x, -1.f);
		BOOST_CHECK_EQUAL(result.y, 0.f);
		BOOST_CHECK_EQUAL(result.z, 0.f);
	}
	BOOST_AUTO_TEST_CASE(MoveOBB)
	{
		OBB obb = OBB(DirectX::XMFLOAT4(0.f,0.f,0.f,1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
		DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(-3.f, 1.f, 2.f);
		DirectX::XMFLOAT4X4 mtrans;
		DirectX::XMStoreFloat4x4(&mtrans, trans);
		obb.updatePosition(mtrans);
		obb.updatePosition(mtrans);

		BOOST_CHECK_EQUAL(obb.getPosition().x, -6.f);
		BOOST_CHECK_EQUAL(obb.getPosition().y, 2.f);
		BOOST_CHECK_EQUAL(obb.getPosition().z, 4.f);

		obb.updatePosition(mtrans);
		BOOST_CHECK_EQUAL(obb.getPosition().x, -9.f);
		BOOST_CHECK_EQUAL(obb.getPosition().y, 3.f);
		BOOST_CHECK_EQUAL(obb.getPosition().z, 6.f);

		obb.setPosition(DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f));

		BOOST_CHECK_EQUAL(obb.getPosition().x, 0.f);
		BOOST_CHECK_EQUAL(obb.getPosition().y, 0.f);
		BOOST_CHECK_EQUAL(obb.getPosition().z, 0.f);
	}
	BOOST_AUTO_TEST_CASE(scaleOBB)
	{
		OBB obb = OBB(DirectX::XMFLOAT4(0.f,0.f,0.f,1.f), DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
		
		BOOST_CHECK_EQUAL(obb.getExtents().x, 1.f); 
		BOOST_CHECK_EQUAL(obb.getExtents().y, 1.f); 
		BOOST_CHECK_EQUAL(obb.getExtents().z, 1.f); 

		obb.scale(DirectX::XMVectorSet(2.f, 2.f, 2.f, 0.f));

		BOOST_CHECK_EQUAL(obb.getExtents().x, 2.f); 
		BOOST_CHECK_EQUAL(obb.getExtents().y, 2.f); 
		BOOST_CHECK_EQUAL(obb.getExtents().z, 2.f); 
	}


BOOST_AUTO_TEST_SUITE_END()