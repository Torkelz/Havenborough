#include <boost/test/unit_test.hpp>
#include "../../../Client/Source/Input/Input.h"

class InputTranslatorStub : public InputTranslator
{
public:
	bool recordFunctionIsSet() { return m_RecordFunction; }
	void sendEvent(InputRecord& r) { m_RecordFunction(r); };
};

BOOST_AUTO_TEST_SUITE(InputTests)

BOOST_AUTO_TEST_CASE(TestCreateInput)
{
	InputTranslatorStub* rawTranslator = new InputTranslatorStub;
	InputTranslatorStub::ptr fakeTranslator(rawTranslator);

	Input input;
	input.init(std::move(fakeTranslator));

	BOOST_CHECK(rawTranslator->recordFunctionIsSet());

	input.destroy();
}

BOOST_AUTO_TEST_CASE(TestInputOnFrame)
{
	InputTranslatorStub* rawTranslator = new InputTranslatorStub;
	InputTranslatorStub::ptr fakeTranslator(rawTranslator);

	Input input;
	input.init(std::move(fakeTranslator));

	InputRecord rec;
	BOOST_REQUIRE(rawTranslator->recordFunctionIsSet());
	rawTranslator->sendEvent(rec);
	BOOST_CHECK_EQUAL(input.getFrameInputs().size(), 1);
	input.onFrame();
	BOOST_CHECK_EQUAL(input.getFrameInputs().size(), 0);

	input.destroy();
}

BOOST_AUTO_TEST_SUITE_END()
