#include <boost\test\unit_test.hpp>
#include "..\..\Physics\include\Sphere.h"

BOOST_AUTO_TEST_SUITE(SphereTesting)

BOOST_AUTO_TEST_CASE(setRadius)
{
	Sphere* sphere = new Sphere(5.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));

	BOOST_CHECK_EQUAL(sphere->getRadius(), 5.f);
	BOOST_CHECK_EQUAL(sphere->getSqrRadius(), 25.f);

	sphere->setRadius(10.f);
	
	BOOST_CHECK_EQUAL(sphere->getRadius(), 10.f);
	BOOST_CHECK_EQUAL(sphere->getSqrRadius(), 100.f);

	delete sphere;
}

BOOST_AUTO_TEST_CASE(updatePositionRelative)
{
	Sphere* sphere = new Sphere(5.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));

	BOOST_CHECK_EQUAL(sphere->getPosition().x, 0.f);
	BOOST_CHECK_EQUAL(sphere->getPosition().y, 0.f);
	BOOST_CHECK_EQUAL(sphere->getPosition().z, 0.f);

	DirectX::XMFLOAT4X4 temp;

	DirectX::XMStoreFloat4x4(&temp, DirectX::XMMatrixTranslation(1.0f, 10.f, 5.f));

	sphere->updatePosition(temp);
	sphere->updatePosition(temp);

	BOOST_CHECK_EQUAL(sphere->getPosition().x, 2.f);
	BOOST_CHECK_EQUAL(sphere->getPosition().y, 20.f);
	BOOST_CHECK_EQUAL(sphere->getPosition().z, 10.f);

	delete sphere;
}

BOOST_AUTO_TEST_CASE(updatePositionAbsolute)
{
	Sphere* sphere = new Sphere(5.f, DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));

	DirectX::XMFLOAT4 temp = DirectX::XMFLOAT4(0.f, 10.f, 0.f, 1.f);

	sphere->updatePosition(temp);
	sphere->updatePosition(temp);

	BOOST_CHECK_EQUAL(sphere->getPosition().x, 0.f);
	BOOST_CHECK_EQUAL(sphere->getPosition().y, 10.f);
	BOOST_CHECK_EQUAL(sphere->getPosition().z, 0.f);

	delete sphere;
}

BOOST_AUTO_TEST_SUITE_END()