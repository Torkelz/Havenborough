#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>

int add(int pI, int pJ) { return pI + pJ; }

BOOST_AUTO_TEST_CASE(myTest)
{
	BOOST_CHECK_EQUAL(add(2, 2), 4);
}