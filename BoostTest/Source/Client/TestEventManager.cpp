#include <boost/test/unit_test.hpp>
#include <EventManager.h>
#include <EventData.h>
#include "../../Client/Source/ClientExceptions.h"
#include <string>
#include <conio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Animation.h"

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
BOOST_AUTO_TEST_CASE(LightEventDataTest)
{
	LightClass light;
	light.position = Vector3(100.f, 10.0f, 10.0f);
	std::shared_ptr<LightEventData> eventData(new LightEventData(light));

	BOOST_CHECK(eventData->getName() == "LightEvent");
	BOOST_CHECK(eventData->getEventType() == 0x748d2b5a);

	LightClass newLight;
	newLight = eventData->getLight();
	BOOST_CHECK(newLight.position == Vector3(100.f, 10.0f, 10.0f));


	std::shared_ptr<LightEventData> newEventData = std::static_pointer_cast<LightEventData>(eventData->copy());
	newLight = newEventData->getLight();
	BOOST_CHECK(newLight.position == Vector3(100.f, 10.0f, 10.0f));
}

BOOST_AUTO_TEST_CASE(RemoveLightEventDataTest)
{
	LightClass::Id lightId = 2;
	std::shared_ptr<RemoveLightEventData> eventData(new RemoveLightEventData(lightId));

	BOOST_CHECK(eventData->getName() == "RemoveLightEvent");
	BOOST_CHECK(eventData->getEventType() == 0x128d2b5a);

	BOOST_CHECK(eventData->getId() == 2);

	std::shared_ptr<RemoveLightEventData> newEventData = std::static_pointer_cast<RemoveLightEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getId() == 2);
}

BOOST_AUTO_TEST_CASE(CreateMeshEventDataTest)
{
	std::shared_ptr<CreateMeshEventData> eventData(new CreateMeshEventData(2, "test", Vector3(2.0f, 1.0f, 2.0f),
		Vector3(0.0f, 0.5f, 0.5f), "testStyle"));

	BOOST_CHECK(eventData->getName() == "CreateMeshEvent");
	BOOST_CHECK(eventData->getEventType() == 0xdeadbeef);

	BOOST_CHECK(eventData->getMeshName() == "test");
	BOOST_CHECK(eventData->getId() == 2);
	BOOST_CHECK(eventData->getScale() == Vector3(2.0f, 1.0f, 2.0f));
	BOOST_CHECK(eventData->getColorTone() == Vector3(0.0f, 0.5f, 0.5f));
	BOOST_CHECK_EQUAL(eventData->getStyle(), "testStyle");

	std::shared_ptr<CreateMeshEventData> newEventData = std::static_pointer_cast<CreateMeshEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getMeshName() == "test");
	BOOST_CHECK(newEventData->getId() == 2);
	BOOST_CHECK(newEventData->getScale() == Vector3(2.0f, 1.0f, 2.0f));
	BOOST_CHECK(newEventData->getColorTone() == Vector3(0.0f, 0.5f, 0.5f));
	BOOST_CHECK_EQUAL(newEventData->getStyle(), "testStyle");
}

BOOST_AUTO_TEST_CASE(RemoveMeshEventDataTest)
{
	std::shared_ptr<RemoveMeshEventData> eventData(new RemoveMeshEventData(2));

	BOOST_CHECK(eventData->getName() == "RemoveMeshEvent");
	BOOST_CHECK(eventData->getEventType() == 0xdeadebbe);

	BOOST_CHECK(eventData->getId() == 2);

	std::shared_ptr<RemoveMeshEventData> newEventData = std::static_pointer_cast<RemoveMeshEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getId() == 2);
}

BOOST_AUTO_TEST_CASE(UpdateModelPositionEventDataTest)
{
	std::shared_ptr<UpdateModelPositionEventData> eventData(new UpdateModelPositionEventData(2, Vector3(10.0f, 0.0f, 5.0f)));

	BOOST_CHECK(eventData->getName() == "UpdateModelPositionEvent");
	BOOST_CHECK(eventData->getEventType() == 0x77dd2b5b);

	BOOST_CHECK(eventData->getId() == 2);
	BOOST_CHECK(eventData->getPosition() == Vector3(10.0f, 0.0f, 5.0f));

	std::shared_ptr<UpdateModelPositionEventData> newEventData = std::static_pointer_cast<UpdateModelPositionEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getId() == 2);
	BOOST_CHECK(newEventData->getPosition() == Vector3(10.0f, 0.0f, 5.0f));
}

BOOST_AUTO_TEST_CASE(UpdateModelScaleEventDataTest)
{
	std::shared_ptr<UpdateModelScaleEventData> eventData(new UpdateModelScaleEventData(2, Vector3(2.0f, 2.0f, 2.0f)));

	BOOST_CHECK(eventData->getName() == "UpdateModelScaleEvent");
	BOOST_CHECK(eventData->getEventType() == 0x77dd2b5c);

	BOOST_CHECK(eventData->getId() == 2);
	BOOST_CHECK(eventData->getScale() == Vector3(2.0f, 2.0f, 2.0f));

	std::shared_ptr<UpdateModelScaleEventData> newEventData = std::static_pointer_cast<UpdateModelScaleEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getId() == 2);
	BOOST_CHECK(newEventData->getScale() == Vector3(2.0f, 2.0f, 2.0f));
}

BOOST_AUTO_TEST_CASE(UpdateModelRotationEventDataTest)
{
	std::shared_ptr<UpdateModelRotationEventData> eventData(new UpdateModelRotationEventData(2, Vector3(5.0f, 0.0f, 0.0f)));

	BOOST_CHECK(eventData->getName() == "UpdateModelRotationEvent");
	BOOST_CHECK(eventData->getEventType() == 0x77dd2b5d);

	BOOST_CHECK(eventData->getId() == 2);
	BOOST_CHECK(eventData->getRotation() == Vector3(5.0f, 0.0f, 0.0f));

	std::shared_ptr<UpdateModelRotationEventData> newEventData = std::static_pointer_cast<UpdateModelRotationEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getId() == 2);
	BOOST_CHECK(newEventData->getRotation() == Vector3(5.0f, 0.0f, 0.0f));
}

BOOST_AUTO_TEST_CASE(UpdateAnimationEventDataTest)
{
	std::vector<DirectX::XMFLOAT4X4> matrix;
	matrix.push_back(DirectX::XMFLOAT4X4(1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4));
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
	Animation anim;
	std::shared_ptr<UpdateAnimationEventData> eventData(new UpdateAnimationEventData(2, matrix, anim.getAnimationData(), world));

	BOOST_CHECK(eventData->getName() == "UpdateAnimationEvent");
	BOOST_CHECK(eventData->getEventType() == 0x14dd2b5d);

	BOOST_CHECK(eventData->getId() == 2);
	BOOST_CHECK(eventData->getAnimationData().size() == 1);

	std::shared_ptr<UpdateAnimationEventData> newEventData = std::static_pointer_cast<UpdateAnimationEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getId() == 2);
	BOOST_CHECK(newEventData->getAnimationData().size() == 1);
}

BOOST_AUTO_TEST_CASE(GameStartedEventDataTest)
{
	std::shared_ptr<GameStartedEventData> eventData(new GameStartedEventData());

	BOOST_CHECK(eventData->getName() == "GameStartedEvent");
	BOOST_CHECK(eventData->getEventType() == 0x38ae3f31);

	std::shared_ptr<GameStartedEventData> newEventData = std::static_pointer_cast<GameStartedEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getName() == "GameStartedEvent");
}

BOOST_AUTO_TEST_CASE(QuitGameEventDataTest)
{
	std::shared_ptr<QuitGameEventData> eventData(new QuitGameEventData());

	BOOST_CHECK(eventData->getName() == "QuitGameEvent");
	BOOST_CHECK(eventData->getEventType() == 0x846e56eb);

	std::shared_ptr<QuitGameEventData> newEventData = std::static_pointer_cast<QuitGameEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getName() == "QuitGameEvent");
}

BOOST_AUTO_TEST_CASE(MouseEventDataLockTest)
{
	std::shared_ptr<MouseEventDataLock> eventData(new MouseEventDataLock(true));

	BOOST_CHECK(eventData->getName() == "MouseEventDataLock");
	BOOST_CHECK(eventData->getEventType() == 0x72dd2b3a);

	BOOST_CHECK(eventData->getLockState());

	std::shared_ptr<MouseEventDataLock> newEventData = std::static_pointer_cast<MouseEventDataLock>(eventData->copy());
	BOOST_CHECK(newEventData->getLockState());
}

BOOST_AUTO_TEST_CASE(MouseEventDataShowTest)
{
	std::shared_ptr<MouseEventDataShow> eventData(new MouseEventDataShow(false));

	BOOST_CHECK(eventData->getName() == "MouseEventDataShow");
	BOOST_CHECK(eventData->getEventType() == 0x22dd2b3a);

	BOOST_CHECK(!eventData->getShowState());

	std::shared_ptr<MouseEventDataShow> newEventData = std::static_pointer_cast<MouseEventDataShow>(eventData->copy());
	BOOST_CHECK(!newEventData->getShowState());
}

BOOST_AUTO_TEST_CASE(ChangeColorToneEventTest)
{
	std::shared_ptr<ChangeColorToneEvent> eventData(new ChangeColorToneEvent(3, Vector3(1.0f, 0.0f, 0.0f)));

	BOOST_CHECK(eventData->getName() == "ChangeColorToneEvent");
	BOOST_CHECK(eventData->getEventType() == 0xbabbab3a);

	BOOST_CHECK(eventData->getMeshId() ==  3);
	BOOST_CHECK(eventData->getColorTone() == Vector3(1.0f, 0.0f, 0.0f));

	std::shared_ptr<ChangeColorToneEvent> newEventData = std::static_pointer_cast<ChangeColorToneEvent>(eventData->copy());
	BOOST_CHECK(newEventData->getMeshId() == 3);
	BOOST_CHECK(newEventData->getColorTone() == Vector3(1.0f, 0.0f, 0.0f));
}

BOOST_AUTO_TEST_CASE(CreateParticleEventDataTest)
{
	std::shared_ptr<CreateParticleEventData> eventData(new CreateParticleEventData(3, "test", Vector3(1.0f, 0.0f, 0.0f)));

	BOOST_CHECK(eventData->getName() == "CreateParticleEvent");
	BOOST_CHECK(eventData->getEventType() == 0x54456edb);

	BOOST_CHECK(eventData->getId() == 3);
	BOOST_CHECK(eventData->getEffectName() ==  "test");
	BOOST_CHECK(eventData->getPosition() == Vector3(1.0f, 0.0f, 0.0f));

	std::shared_ptr<CreateParticleEventData> newEventData = std::static_pointer_cast<CreateParticleEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getId() == 3);
	BOOST_CHECK(newEventData->getEffectName() ==  "test");
	BOOST_CHECK(newEventData->getPosition() == Vector3(1.0f, 0.0f, 0.0f));
}

BOOST_AUTO_TEST_CASE(RemoveParticleEventDataTest)
{
	std::shared_ptr<RemoveParticleEventData> eventData(new RemoveParticleEventData(3));

	BOOST_CHECK(eventData->getName() == "RemoveParticleEvent");
	BOOST_CHECK(eventData->getEventType() == 0x82544aeb);

	BOOST_CHECK(eventData->getId() == 3);

	std::shared_ptr<RemoveParticleEventData> newEventData = std::static_pointer_cast<RemoveParticleEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getId() == 3);
}

BOOST_AUTO_TEST_CASE(UpdateParticlePositionEventDataTest)
{
	std::shared_ptr<UpdateParticlePositionEventData> eventData(new UpdateParticlePositionEventData(1, Vector3(5.0f, 0.0f, 0.0f)));

	BOOST_CHECK(eventData->getName() == "UpdateParticlePositionEvent");
	BOOST_CHECK(eventData->getEventType() == 0xd02a90fc);

	BOOST_CHECK(eventData->getId() == 1);
	BOOST_CHECK(eventData->getPosition() == Vector3(5.0f, 0.0f, 0.0f));

	std::shared_ptr<UpdateParticlePositionEventData> newEventData = std::static_pointer_cast<UpdateParticlePositionEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getId() == 1);
	BOOST_CHECK(newEventData->getPosition() == Vector3(5.0f, 0.0f, 0.0f));
}

BOOST_AUTO_TEST_CASE(RemoveActorEventDataTest)
{
	Actor::Id actorId = 1;
	std::shared_ptr<RemoveActorEventData> eventData(new RemoveActorEventData(actorId));

	BOOST_CHECK(eventData->getName() == "RemoveActorEvent");
	BOOST_CHECK(eventData->getEventType() == 0x91615dff);

	BOOST_CHECK(eventData->getActorId() == actorId);

	std::shared_ptr<RemoveActorEventData> newEventData = std::static_pointer_cast<RemoveActorEventData>(eventData->copy());
	BOOST_CHECK(newEventData->getActorId() == actorId);
}

BOOST_AUTO_TEST_SUITE_END()