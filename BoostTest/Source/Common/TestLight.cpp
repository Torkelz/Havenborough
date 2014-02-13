#include <boost/test/unit_test.hpp>
#include "Light.h"
BOOST_AUTO_TEST_SUITE(TestLight)

BOOST_AUTO_TEST_CASE(Common_Light_CreateTest)
{
	Light light;
	
	light = Light::createDirectionalLight(Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f));
	BOOST_CHECK(light.direction == Vector3(1.0f, 1.0f, 1.0f));
	BOOST_CHECK(light.color == Vector3(1.0f, 0.0f, 0.0f));
	BOOST_CHECK(light.type == Light::Type::DIRECTIONAL);

	light = Light::createSpotLight(Vector3(10.0f, 4.0f, 6.0f), Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.6f, 0.3f),
		100.0f, Vector3(0.0f, 0.0f, 1.0f));
	BOOST_CHECK(light.position == Vector3(10.0f, 4.0f, 6.0f));
	BOOST_CHECK(light.direction == Vector3(-1.0f, -1.0f, 0.0f));
	BOOST_CHECK(light.spotlightAngles == Vector2(0.6f, 0.3f));
	BOOST_CHECK(light.range == 100.0f);
	BOOST_CHECK(light.color == Vector3(0.0f, 0.0f, 1.0f));
	BOOST_CHECK(light.type == Light::Type::SPOT);


	light = Light::createPointLight(Vector3(4.0f, 4.0f, 1.0f), 200.0f, Vector3(0.0f, 1.0f, 0.0f));
	BOOST_CHECK(light.position == Vector3(4.0f, 4.0f, 1.0f));
	BOOST_CHECK(light.range == 200.0f);
	BOOST_CHECK(light.color == Vector3(0.0f, 1.0f, 0.0f));
	BOOST_CHECK(light.type == Light::Type::POINT);
}


BOOST_AUTO_TEST_SUITE_END()