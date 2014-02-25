#include <boost/test/unit_test.hpp>
#include "../../Graphics/Source/DeferredRenderer.h"
#include "../../Graphics/Source/GraphicsExceptions.h"

BOOST_AUTO_TEST_SUITE(TestDeferredRenderer)

BOOST_AUTO_TEST_CASE(TestDef)
{
	DeferredRenderer *deferred;
	BOOST_CHECK_NO_THROW(deferred = new DeferredRenderer());
	
	BOOST_CHECK_THROW(deferred->initialize(nullptr, nullptr, nullptr, 1280, 720, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 10, 170.0f, 1000.0f), DeferredRenderException);

	BOOST_CHECK_NO_THROW(SAFE_DELETE(deferred));
}

BOOST_AUTO_TEST_SUITE_END()