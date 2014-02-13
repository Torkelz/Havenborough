#include <boost/test/unit_test.hpp>
#include "Utilities/XMFloatUtil.h"
BOOST_AUTO_TEST_SUITE(TestXMFloatUtil)

BOOST_AUTO_TEST_CASE(XMFloatUtil_Vector2)
{
	Vector2 vector;
	BOOST_CHECK(vector.x == 0);
	BOOST_CHECK(vector.y == 0);
}
	

BOOST_AUTO_TEST_SUITE_END()