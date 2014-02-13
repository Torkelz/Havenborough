#include <boost/test/unit_test.hpp>
#include "../../Graphics/Source/SkyDome.h"
#include "../../Graphics/Source/GraphicsExceptions.h"

BOOST_AUTO_TEST_SUITE(SkydomeTest)

using namespace DirectX;

BOOST_AUTO_TEST_CASE(checkBuildGeoSphere)
{
	SkyDome sky;

	BOOST_CHECK_THROW(sky.init(nullptr, nullptr,nullptr, 50.f), GraphicsException);

}

BOOST_AUTO_TEST_SUITE_END()