#include <boost/test/unit_test.hpp>
#include "../../../Client/Source/Input/Input.h"
#include "../../../Client/Source/MyExceptions.h"

class InputTranslatorStub : public InputTranslator
{
public:
	bool recordFunctionIsSet() { return m_RecordFunction; }
	void sendEvent(InputRecord& r) { m_RecordFunction(r); }
	bool handleFakeKeyboardInput(const RAWKEYBOARD& key)
		{ return handleKeyboardInput(key); }
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
	BOOST_CHECK_EQUAL(input.getCurrentState().size(), 1);
	input.onFrame();
	BOOST_CHECK_EQUAL(input.getFrameInputs().size(), 0);
	BOOST_CHECK_EQUAL(input.getCurrentState().size(), 1);

	input.destroy();
}

BOOST_AUTO_TEST_CASE(TestInputRegisteredKey)
{
	InputTranslatorStub* rawTranslator = new InputTranslatorStub;
	InputTranslatorStub::ptr fakeTranslator(rawTranslator);

	static const std::string spaceAction("testSpace");
	static const std::string pAction("testP");

	fakeTranslator->addKeyboardMapping(VK_SPACE, spaceAction);
	fakeTranslator->addKeyboardMapping('P', "testP");

	Input input;
	input.init(std::move(fakeTranslator));

	BOOST_REQUIRE(rawTranslator->recordFunctionIsSet());

	RAWKEYBOARD spacekey = {};
	spacekey.Flags = RI_KEY_MAKE;
	spacekey.VKey = VK_SPACE;

	RAWKEYBOARD pKey = {};
	pKey.Flags = RI_KEY_BREAK;
	pKey.VKey = 'P';

	BOOST_CHECK_EQUAL(rawTranslator->handleFakeKeyboardInput(spacekey), true);

	BOOST_REQUIRE_EQUAL(input.getFrameInputs().size(), 1);
	BOOST_CHECK_EQUAL(input.getFrameInputs()[0].m_Action, spaceAction);
	BOOST_CHECK_EQUAL(input.getFrameInputs()[0].m_Value, 1.f);

	BOOST_CHECK_EQUAL(input.getCurrentState().size(), 1);
	BOOST_CHECK(input.getCurrentState().hasRecord(spaceAction));
	BOOST_CHECK_EQUAL(input.getCurrentState().getValue(spaceAction), 1.f);
	
	BOOST_CHECK(!input.getCurrentState().hasRecord(pAction));
	BOOST_CHECK_EQUAL(input.getCurrentState().getValue(pAction), 0.f);

	input.onFrame();

	BOOST_CHECK_EQUAL(input.getFrameInputs().size(), 0);

	BOOST_CHECK_EQUAL(input.getCurrentState().size(), 1);
	BOOST_CHECK(input.getCurrentState().hasRecord(spaceAction));
	BOOST_CHECK_EQUAL(input.getCurrentState().getValue(spaceAction), 1.f);
	
	BOOST_CHECK(!input.getCurrentState().hasRecord(pAction));
	BOOST_CHECK_EQUAL(input.getCurrentState().getValue(pAction), 0.f);

	BOOST_CHECK_EQUAL(rawTranslator->handleFakeKeyboardInput(pKey), true);
	BOOST_CHECK_EQUAL(rawTranslator->handleFakeKeyboardInput(spacekey), true);

	BOOST_REQUIRE_EQUAL(input.getFrameInputs().size(), 2);
	BOOST_CHECK_EQUAL(input.getFrameInputs()[0].m_Action, pAction);
	BOOST_CHECK_EQUAL(input.getFrameInputs()[0].m_Value, 0.f);

	BOOST_CHECK_EQUAL(input.getFrameInputs()[1].m_Action, spaceAction);
	BOOST_CHECK_EQUAL(input.getFrameInputs()[1].m_Value, 1.f);

	BOOST_CHECK_EQUAL(input.getCurrentState().size(), 2);
	BOOST_CHECK(input.getCurrentState().hasRecord(spaceAction));
	BOOST_CHECK_EQUAL(input.getCurrentState().getValue(spaceAction), 1.f);
	
	BOOST_CHECK(input.getCurrentState().hasRecord(pAction));
	BOOST_CHECK_EQUAL(input.getCurrentState().getValue(pAction), 0.f);

	input.onFrame();

	BOOST_CHECK_EQUAL(input.getCurrentState().size(), 2);

	input.destroy();

	BOOST_CHECK_EQUAL(input.getCurrentState().size(), 0);
}

BOOST_AUTO_TEST_CASE(TestInputTranslatorExc)
{
	InputTranslator inTrans;
	BOOST_CHECK_THROW(inTrans.init(nullptr), InvalidArgument);
}

BOOST_AUTO_TEST_SUITE_END()
