#include <boost\test\unit_test.hpp>
#include "..\..\Physics\include\AABB.h"

BOOST_AUTO_TEST_SUITE(AABBTesting)

BOOST_AUTO_TEST_CASE(buildAABB)
{
	DirectX::XMFLOAT4 bot = DirectX::XMFLOAT4(0.5f, 1.5f, 3.f, 1.f);
	DirectX::XMFLOAT4 top = DirectX::XMFLOAT4(3.f, 5.f, 8.f, 1.f);

	AABB* aabb = new AABB(bot, top);

	BOOST_CHECK_EQUAL(aabb->getPosition()->x, 0.5f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->y, 1.5f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->z, 3.f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->w, 1.f);

	BOOST_CHECK_EQUAL(aabb->getHalfDiagonal()->x, 3.f);
	BOOST_CHECK_EQUAL(aabb->getHalfDiagonal()->y, 5.f);
	BOOST_CHECK_EQUAL(aabb->getHalfDiagonal()->z, 8.f);
	BOOST_CHECK_EQUAL(aabb->getHalfDiagonal()->w, 0.f);

	delete aabb;
}

BOOST_AUTO_TEST_CASE(buildAABBBotSameAsTop)
{
	DirectX::XMFLOAT4 bot = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);

	AABB* aabb = new AABB(bot, bot);

	BOOST_CHECK_EQUAL(aabb->getPosition()->x, 0.f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->y, 0.f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->z, 0.f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->w, 1.f);

	delete aabb;
}

BOOST_AUTO_TEST_CASE(updatePosition)
{
	DirectX::XMFLOAT4 bot = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	DirectX::XMFLOAT4 top = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);

	AABB* aabb = new AABB(bot, top);

	BOOST_CHECK_EQUAL(aabb->getPosition()->x, 0.0f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->y, 0.0f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->z, 0.0f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->w, 1.f);

	DirectX::XMFLOAT4X4 temp;

	DirectX::XMStoreFloat4x4(&temp, DirectX::XMMatrixTranslation(1.0f, 10.f, 5.f));

	aabb->updatePosition(temp);
	aabb->updatePosition(temp);

	BOOST_CHECK_EQUAL(aabb->getPosition()->x, 2.f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->y, 20.f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->z, 10.f);
	BOOST_CHECK_EQUAL(aabb->getPosition()->w, 1.f);

	delete aabb;
}

BOOST_AUTO_TEST_SUITE_END()