#include <boost/test/unit_test.hpp>
#include "../../Graphics/Source/ScreenRenderer.h"
#include "../../Graphics/Source/GraphicsExceptions.h"

BOOST_AUTO_TEST_SUITE(TestScreenRenderer)

BOOST_AUTO_TEST_CASE(ScreenRenderer_Initialize)
{
	ScreenRenderer screenRenderer;

	BOOST_CHECK_THROW(screenRenderer.initialize(nullptr, nullptr, nullptr, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
		nullptr, nullptr), GraphicsException);

	BOOST_CHECK_NO_THROW(screenRenderer.add2D_Object(Renderable2D(nullptr)));
}

BOOST_AUTO_TEST_SUITE_END()