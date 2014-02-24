#include <boost/test/unit_test.hpp>
#include "Utilities/XMFloatUtil.h"
BOOST_AUTO_TEST_SUITE(TestXMFloatUtil)

BOOST_AUTO_TEST_CASE(XMFloatUtil_Vector2)
{
	Vector2 vector;
	BOOST_CHECK(vector.x == 0.0f);
	BOOST_CHECK(vector.y == 0.0f);
	
	vector.x = 1.0f;
	BOOST_CHECK(vector == Vector2(1.0f, 0.0f));
}

BOOST_AUTO_TEST_CASE(XMFloatUtil_Vector3)
{
	Vector3 vector;
	BOOST_CHECK(vector.x == 0.0f);
	BOOST_CHECK(vector.y == 0.0f);
	BOOST_CHECK(vector.z == 0.0f);

	vector.y = 1.0f;
	vector.z = 5.0f;

	Vector3 newVector = vector;
	BOOST_CHECK(newVector == vector);

	newVector = newVector + vector;
	BOOST_CHECK(newVector == Vector3(0.0f, 2.0f, 10.0f));

	newVector = newVector - vector;
	BOOST_CHECK(newVector == Vector3(0.0f, 1.0f, 5.0f));
	
	newVector = newVector * 2.0f;
	BOOST_CHECK(newVector == Vector3(0.0f, 2.0f, 10.0f));
}

BOOST_AUTO_TEST_CASE(XMFloatUtil_Vector4)
{
	Vector4 vector;
	BOOST_CHECK(vector.x == 0.0f);
	BOOST_CHECK(vector.y == 0.0f);
	BOOST_CHECK(vector.z == 0.0f);
	BOOST_CHECK(vector.w == 0.0f);

	Vector4 newVector = Vector4(1.0f, 0.5f, 1.0f, 0.5f);
	vector = vector - newVector;
	BOOST_CHECK(vector.x == -1.0f);
	BOOST_CHECK(vector.y == -0.5f);
	BOOST_CHECK(vector.z == -1.0f);
	BOOST_CHECK(vector.w == -0.5f);

	BOOST_CHECK(vector.xyz() == Vector3(-1.0f, -0.5f, -1.0f));
}

BOOST_AUTO_TEST_SUITE_END()