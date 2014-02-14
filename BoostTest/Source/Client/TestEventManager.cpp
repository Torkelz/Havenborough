#include <boost/test/unit_test.hpp>
#include <EventManager.h>
#include <EventData.h>
#include "../../Client/Source/ClientExceptions.h"
#include <string>
#include <conio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

BOOST_AUTO_TEST_SUITE(TestEventManager)

bool testFlag = false;

void testDelegate(IEventData::Ptr in)
{
	testFlag = true;
}
void testDelegateWhile(IEventData::Ptr in)
{
	Sleep(2);
}

class dummy
{
private:
	bool a;
public:
	dummy(){a = false;}
	explicit dummy(bool p) : a(p) {}

	void setValue(IEventData::Ptr in){a = true;}
	bool getValue(){return a;}
};

/**
* Example test of event data
*/
BOOST_AUTO_TEST_CASE(TestEventDataTest)
{
	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));

	BOOST_CHECK(Harbinger->directInterventionIsNecessary() == true);
	
	std::string type(Harbinger->getName());
	BOOST_CHECK(type == "TestEvent");

	IEventData::Type eventCheck(0x77dd2b3a);
	BOOST_CHECK(Harbinger->getEventType() == eventCheck);

	std::shared_ptr<TestEventData> Sovereign = std::static_pointer_cast<TestEventData>(Harbinger->copy());
	BOOST_CHECK(Sovereign->directInterventionIsNecessary() == true);
}


/**
* EventManager tests
*/
BOOST_AUTO_TEST_CASE(EventManager_AddListener)
{
	EventManager testEventManager;
	EventListenerDelegate delegater(&TestEventManager::testDelegate);
	IEventData::Type eventCheck(0x77dd2b3a);
	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));

	//Should throw since event type already has been added.
	BOOST_CHECK_THROW(testEventManager.addListener(delegater, eventCheck), EventException);
}

BOOST_AUTO_TEST_CASE(EventManager_RemoveListener)
{
	EventManager testEventManager;
	EventListenerDelegate delegater(&TestEventManager::testDelegate);
	IEventData::Type eventCheck(0x77dd2b3a);
	IEventData::Type eventCheckNotAdded(0x77dd2b3b);

	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));
	
	BOOST_CHECK(testEventManager.removeListener(delegater, eventCheckNotAdded) == false);
	BOOST_CHECK(testEventManager.removeListener(delegater, eventCheck) == true);
	BOOST_CHECK(testEventManager.removeListener(delegater, eventCheck) == false);
}

BOOST_AUTO_TEST_CASE(EventManager_TriggerTriggerEvent)
{
	EventManager testEventManager;
	EventListenerDelegate delegater(&TestEventManager::testDelegate);
	IEventData::Type eventCheck(0x77dd2b3a);

	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));
	BOOST_CHECK(testEventManager.triggerTriggerEvent(Harbinger) == false);

	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));

	BOOST_CHECK(testEventManager.triggerTriggerEvent(Harbinger) == true);
}

BOOST_AUTO_TEST_CASE(EventManager_QueueEvent)
{
	EventManager testEventManager;
	EventListenerDelegate delegater(&TestEventManager::testDelegate);
	IEventData::Type eventCheck(0x77dd2b3a);

	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == false);

	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
}

BOOST_AUTO_TEST_CASE(EventManager_AbortEvent)
{
	EventManager testEventManager;
	EventListenerDelegate delegater(&TestEventManager::testDelegate);
	IEventData::Type eventCheck(0x77dd2b3a);

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

BOOST_AUTO_TEST_CASE(EventManager_TickUpdate)
{
	EventManager testEventManager;
	EventListenerDelegate delegater(&TestEventManager::testDelegate);
	EventListenerDelegate delegaterWhile(&TestEventManager::testDelegateWhile);

	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));
	IEventData::Type eventCheck(0x77dd2b3a);
	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));

	std::chrono::milliseconds harvestTime(10);
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.processEvents(harvestTime) == true);
	BOOST_CHECK(testFlag == true);
	BOOST_CHECK(testEventManager.abortEvent(eventCheck, true) == false);
	BOOST_CHECK_NO_THROW(testEventManager.removeListener(delegater, eventCheck));

	std::chrono::milliseconds processTime(4);
	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegaterWhile, eventCheck));
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.processEvents(processTime) == false);
}

BOOST_AUTO_TEST_CASE(EventManager_TickUpdateWithMemberFunction)
{
	EventManager testEventManager;
	dummy d;
	EventListenerDelegate delegater(&d,&dummy::setValue);

	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));
	IEventData::Type eventCheck(0x77dd2b3a);
	BOOST_CHECK_NO_THROW(testEventManager.addListener(delegater, eventCheck));

	std::chrono::milliseconds harvestTime(10);
	BOOST_CHECK(testEventManager.queueEvent(Harbinger) == true);
	BOOST_CHECK(testEventManager.processEvents(harvestTime) == true);
	BOOST_CHECK(d.getValue() == true);
}


/**
* EventData tests
*/
BOOST_AUTO_TEST_CASE(TestEventDataTest)
{
	std::shared_ptr<TestEventData> Harbinger(new TestEventData(true));

	BOOST_CHECK(Harbinger->directInterventionIsNecessary() == true);

	std::string name(Harbinger->getName());
	BOOST_CHECK(name == "TestEvent");

	IEventData::Type eventCheck(0x77dd2b3a);
	BOOST_CHECK(Harbinger->getEventType() == eventCheck);

	std::shared_ptr<TestEventData> Sovereign = std::static_pointer_cast<TestEventData>(Harbinger->copy());
	BOOST_CHECK(Sovereign->directInterventionIsNecessary() == true);
}

BOOST_AUTO_TEST_CASE(LightEventDataTest)
{
	Light light;
	light.position = Vector3(100.f, 10.0f, 10.0f);
	std::shared_ptr<LightEventData> eventData(new LightEventData(light));

	BOOST_CHECK(eventData->directInterventionIsNecessary() == true);

	std::string type(eventData->getName());
	BOOST_CHECK(type == "TestEvent");

	IEventData::Type eventCheck(0x748d2b5a);
	BOOST_CHECK(eventData->getEventType() == eventCheck);

	std::shared_ptr<LightEventData> Sovereign = std::static_pointer_cast<LightEventData>(eventData->copy());
	BOOST_CHECK(Sovereign->directInterventionIsNecessary() == true);
}

BOOST_AUTO_TEST_SUITE_END()