#include <boost/test/unit_test.hpp>
#include "../../Client/Source/EventManager.h"
#include "../../Client/Source/EventData.h"
#include "../../Client/Source/ClientExceptions.h"
#include <string>
#include <conio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

BOOST_AUTO_TEST_SUITE(TestEventManager)

bool testFlag = false;

void testDelegate(IEventData::IEventDataPtr in)
{
	testFlag = true;
}
void testDelegateWhile(IEventData::IEventDataPtr in)
{
	Sleep(1);
}

BOOST_AUTO_TEST_CASE(TestEventDataTest)
{
	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));

	BOOST_CHECK(Harbinger->directInterventionIsNecessary() == true);
	
	std::string type(Harbinger->getName());
	BOOST_CHECK(type == "TestEvent");

	IEventData::EventType eventCheck(0x77dd2b3a);
	BOOST_CHECK(Harbinger->getEventType() == eventCheck);

	std::shared_ptr<TestEventData> Sovereign = std::static_pointer_cast<TestEventData>(Harbinger->copy());
	BOOST_CHECK(Sovereign->directInterventionIsNecessary() == true);
}

BOOST_AUTO_TEST_CASE(TestEventManagerAddListener)
{
	EventManager testEventManager;
	IEventManager::EventListenerDelegate delegater = &TestEventManager::testDelegate;
	IEventData::EventType eventCheck(0x77dd2b3a);
	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));

	//Should throw since event type already has been added.
	BOOST_CHECK_THROW(testEventManager.addListener(delegater, eventCheck), EventException);
}

BOOST_AUTO_TEST_CASE(TestEventManagerRemoveListener)
{
	EventManager testEventManager;
	IEventManager::EventListenerDelegate delegater = &TestEventManager::testDelegate;
	IEventData::EventType eventCheck(0x77dd2b3a);
	IEventData::EventType eventCheckNotAdded(0x77dd2b3b);

	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));
	
	BOOST_CHECK(testEventManager.removeListener(delegater, eventCheckNotAdded) == false);
	BOOST_CHECK(testEventManager.removeListener(delegater, eventCheck) == true);
	BOOST_CHECK(testEventManager.removeListener(delegater, eventCheck) == false);
}

BOOST_AUTO_TEST_CASE(TestEventManagerTriggerTriggerEvent)
{
	EventManager testEventManager;
	IEventManager::EventListenerDelegate delegater = &TestEventManager::testDelegate;
	IEventData::EventType eventCheck(0x77dd2b3a);

	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));
	BOOST_CHECK(testEventManager.triggerTriggerEvent(Harbinger) == false);

	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));

	BOOST_CHECK(testEventManager.triggerTriggerEvent(Harbinger) == true);
}

BOOST_AUTO_TEST_CASE(TestEventManagerQueueEvent)
{
	EventManager testEventManager;
	IEventManager::EventListenerDelegate delegater = &TestEventManager::testDelegate;
	IEventData::EventType eventCheck(0x77dd2b3a);

	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == false);

	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
}

BOOST_AUTO_TEST_CASE(TestEventManagerAbortEvent)
{
	EventManager testEventManager;
	IEventManager::EventListenerDelegate delegater = &TestEventManager::testDelegate;
	IEventData::EventType eventCheck(0x77dd2b3a);

	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));

	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.abortEvent(eventCheck, true) == true);
	// Make sure all related events are removed.
	BOOST_CHECK(testEventManager.abortEvent(eventCheck, true) == false);

	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.abortEvent(eventCheck, false) == true);
	// Make sure there is one of the related events left.
	BOOST_CHECK(testEventManager.abortEvent(eventCheck, false) == true);
}

BOOST_AUTO_TEST_CASE(TestEventManagerTickUpdate)
{
	EventManager testEventManager;
	IEventManager::EventListenerDelegate delegater = &TestEventManager::testDelegate;
	IEventManager::EventListenerDelegate delegaterWhile = &TestEventManager::testDelegateWhile;

	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));
	IEventData::EventType eventCheck(0x77dd2b3a);
	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));

	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.tickUpdate(10) == true);
	BOOST_CHECK(testFlag == true);
	BOOST_CHECK(testEventManager.abortEvent(eventCheck, true) == false);
	BOOST_CHECK_NO_THROW(testEventManager.removeListener(delegater, eventCheck));

	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegaterWhile, eventCheck));
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.tickUpdate(10) == false);
}

BOOST_AUTO_TEST_SUITE_END()