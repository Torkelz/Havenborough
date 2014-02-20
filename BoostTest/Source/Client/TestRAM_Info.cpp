#include <boost/test/unit_test.hpp>
#include "../../Client/Source/RAMInfo.h"

BOOST_AUTO_TEST_SUITE(TestRAM_Info)

BOOST_AUTO_TEST_CASE(RAM_InfoTest)
{
	RAMInfo info;

	BOOST_CHECK_NO_THROW(info.update());
	BOOST_CHECK(info.getPhysicalMemoryUsage() > 0);
	BOOST_CHECK(info.getVirtualMemoryUsage() > 0);
}

BOOST_AUTO_TEST_SUITE_END()