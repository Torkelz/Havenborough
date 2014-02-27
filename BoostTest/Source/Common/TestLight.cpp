#include <boost/test/unit_test.hpp>
#include "LightClass.h"
BOOST_AUTO_TEST_SUITE(TestLight)

BOOST_AUTO_TEST_CASE(Common_Light_CreateTest)
{
	LightClass light;
	
	light = LightClass::createDirectionalLight(Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f),1);
	BOOST_CHECK(light.direction == Vector3(1.0f, 1.0f, 1.0f));
	BOOST_CHECK(light.color == Vector3(1.0f, 0.0f, 0.0f));
	BOOST_CHECK(light.type == LightClass::Type::DIRECTIONAL);

	light = LightClass::createSpotLight(Vector3(10.0f, 4.0f, 6.0f), Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.6f, 0.3f),
		100.0f, Vector3(0.0f, 0.0f, 1.0f));
	BOOST_CHECK(light.position == Vector3(10.0f, 4.0f, 6.0f));
	BOOST_CHECK(light.direction == Vector3(-1.0f, -1.0f, 0.0f));
	BOOST_CHECK(light.spotlightAngles == Vector2(0.6f, 0.3f));
	BOOST_CHECK(light.range == 100.0f);
	BOOST_CHECK(light.color == Vector3(0.0f, 0.0f, 1.0f));
	BOOST_CHECK(light.type == LightClass::Type::SPOT);


	light = LightClass::createPointLight(Vector3(4.0f, 4.0f, 1.0f), 200.0f, Vector3(0.0f, 1.0f, 0.0f));
	BOOST_CHECK(light.position == Vector3(4.0f, 4.0f, 1.0f));
	BOOST_CHECK(light.range == 200.0f);
	BOOST_CHECK(light.color == Vector3(0.0f, 1.0f, 0.0f));
	BOOST_CHECK(light.type == LightClass::Type::POINT);
}


BOOST_AUTO_TEST_SUITE_END()