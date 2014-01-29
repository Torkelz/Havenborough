#include <boost/test/unit_test.hpp>
#include "ISound.h"
#include "../../Sound/Source/Sound.h"
#include "../../Common/Source/EventManager.h"

#if _DEBUG
#include <vld.h> 
#endif

BOOST_AUTO_TEST_SUITE(SoundEngine)

class SoundEventIDataPlay : public BaseEventData
{
private: 
	//Parameter for event, can be of any number
	std::string m_SoundId;

public:
	/**
	* Unique identifier for event data type. This one is an example and for testing.
	* E.g. IEventData::Type UniqueEventDataName::sk_EventType(unique_hex);
	*/
	static const IEventData::Type sk_EventType = Type(0x77dd2b43);
	
	explicit SoundEventIDataPlay(std::string p_SoundId) :
		m_SoundId(p_SoundId)
	{
	}

	virtual const IEventData::Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new SoundEventIDataPlay(m_SoundId));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
		p_Out << m_SoundId;
	}

	virtual const char *getName(void) const override
	{
		return "SoundEventIDataPlay";
	}

	/**
	* Used to get the event data. User defined function.
	* Can be of any number of functions.
	*/
	std::string getSoundId(void) const
	{
		return m_SoundId;
	}
};

class SoundEventIDataPause : public BaseEventData
{
private: 
	//Parameter for event, can be of any number
	std::string m_SoundId;
	bool m_Paused;

public:
	/**
	* Unique identifier for event data type. This one is an example and for testing.
	* E.g. IEventData::Type UniqueEventDataName::sk_EventType(unique_hex);
	*/
	static const IEventData::Type sk_EventType = Type(0x77d22b43);
	
	explicit SoundEventIDataPause(std::string p_SoundId, bool p_Paused) :
		m_SoundId(p_SoundId), m_Paused(p_Paused)
	{
	}

	virtual const IEventData::Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new SoundEventIDataPause(m_SoundId, m_Paused));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
		p_Out << m_SoundId << m_Paused;
	}

	virtual const char *getName(void) const override
	{
		return "SoundEventIDataPause";
	}

	/**
	* Used to get the event data. User defined function.
	* Can be of any number of functions.
	*/
	std::string getSoundId(void) const
	{
		return m_SoundId;
	}

	bool getPaused(void) const
	{
		return m_Paused;
	}
};

// ############### Event Functions START ###############
ISound *sound;
void eventPlay(IEventData::Ptr p_Data)
{
	std::shared_ptr<SoundEventIDataPlay> data = std::static_pointer_cast<SoundEventIDataPlay>(p_Data);

	sound->playSound(data->getSoundId().c_str());
}
void eventPause(IEventData::Ptr p_Data)
{
	std::shared_ptr<SoundEventIDataPause> data = std::static_pointer_cast<SoundEventIDataPause>(p_Data);

	sound->pauseSound(data->getSoundId().c_str(), data->getPaused());
}

// ############### Event Functions END ###############

BOOST_AUTO_TEST_CASE(TestSound)
{
	const std::string ID = "2> ";

	BOOST_MESSAGE( "" );

	BOOST_MESSAGE(ID + "Running TestSound...");

	// ############### STEP 1 START ###############
	BOOST_MESSAGE(ID + "Step 1: Checking initialization of the sound engine.");
	sound = ISound::createSound();

	BOOST_CHECK_MESSAGE(sound != nullptr, ID + "The Sound interface pointer equals null. Creation failed.");
	BOOST_MESSAGE(ID + "Starting to initialize the Sound interface...");
	BOOST_CHECK_NO_THROW(sound->initialize());
	// ############### STEP 1 END ###############

	// ############### STEP 2 START ###############
	BOOST_MESSAGE(ID + "Step 2: Checking the load sound function.");
	std::string ts = "TestSound";
	BOOST_MESSAGE(ID + "Adding default sound, 'gun_battle.mp3'...");
	BOOST_CHECK_NO_THROW(sound->loadSound(ts.c_str(), "assets/gun_battle.mp3"));

	BOOST_MESSAGE(ID + "Trying to add non-existent sound, 'gun.mp3'...");
	BOOST_CHECK_THROW(sound->loadSound("FakeSound1", "assets/gun.mp3"), SoundException);
	
	BOOST_MESSAGE(ID + "Trying to add illegal file, 'gun.txt'...");
	BOOST_CHECK_THROW(sound->loadSound("FakeSound2", "assets/gun.txt"), SoundException);
	// ############### STEP 2 END ###############

	// ############### STEP 3 START ###############
	BOOST_MESSAGE(ID + "Step 3: Sound manipulation.");
	BOOST_MESSAGE(ID + "Playing default sound.");
	BOOST_CHECK_NO_THROW(sound->playSound(ts.c_str()));

	BOOST_MESSAGE(ID + "Trying to play an uninitialized sound, 'NoSound'.");
	BOOST_CHECK_THROW(sound->playSound("NoSound"), SoundException);

	BOOST_MESSAGE(ID + "Changing volume on default sound.");
	float previousVolume = -1.0f;
	float currentVolume = -1.0f;
	BOOST_CHECK_NO_THROW(previousVolume = sound->getVolume(ts.c_str()));
	BOOST_CHECK_NO_THROW(sound->setSoundVolume(ts.c_str(), 0.5f));
	BOOST_CHECK_NO_THROW(currentVolume = sound->getVolume(ts.c_str()));
	BOOST_CHECK_MESSAGE(previousVolume != currentVolume && currentVolume == 0.5f, ID + "The volume change failed.");

	BOOST_MESSAGE( ID + "Pausing and unpausing default sound.");
	BOOST_CHECK_NO_THROW(sound->pauseSound(ts.c_str(), true));
	BOOST_CHECK_MESSAGE(sound->getPaused(ts.c_str()) != false, ID + "Pausing the sound failed.");
	BOOST_CHECK_NO_THROW(sound->pauseSound(ts.c_str(), false));
	BOOST_CHECK_MESSAGE(sound->getPaused(ts.c_str()) != true, ID + "Unpausing the sound failed.");

	BOOST_MESSAGE( ID + "Stopping default sound and restarting it.");
	BOOST_CHECK_NO_THROW(sound->stopSound(ts.c_str()));
	BOOST_CHECK_THROW(sound->setSoundVolume(ts.c_str(),0.5f), SoundException);
	BOOST_CHECK_NO_THROW(sound->playSound(ts.c_str()));
	BOOST_CHECK_NO_THROW(sound->setSoundVolume(ts.c_str(),0.5f));

	BOOST_MESSAGE( ID + "Muting default sound and unmuting it.");
	BOOST_CHECK_NO_THROW(sound->addSoundToGroup(ts.c_str(),ISound::ChannelGroup::MUSIC));
	BOOST_CHECK_NO_THROW(sound->muteAll(true));
	BOOST_CHECK_MESSAGE(sound->getMute(ts.c_str()) == true, ID + "Muting the sound failed.");
	BOOST_CHECK_NO_THROW(sound->muteAll(false));
	BOOST_CHECK_MESSAGE(sound->getMute(ts.c_str()) == false, ID + "Unmuting the sound failed.");

	BOOST_MESSAGE( ID + "Trying to pass non-valid number 2 and -1 to volume change functions.");
	BOOST_CHECK_NO_THROW(sound->setSoundVolume(ts.c_str(),-1.0f));
	BOOST_CHECK_NO_THROW(sound->setSoundVolume(ts.c_str(), 2.0f));

	BOOST_MESSAGE( ID + "Trying to pass non-valid number 2 and -1 to pause change functions.");
	BOOST_CHECK_NO_THROW(sound->pauseSound(ts.c_str(),-1));
	BOOST_CHECK_NO_THROW(sound->pauseSound(ts.c_str(), 2));
	// ############### STEP 3 END ###############

	// ############### STEP 4 START ###############
	BOOST_MESSAGE(ID + "Step 4: Group sound manipulations.");
	BOOST_MESSAGE( ID + "default sound is added to a channel group.");
	BOOST_CHECK_NO_THROW(sound->addSoundToGroup(ts.c_str(),ISound::ChannelGroup::MASTER));

	BOOST_MESSAGE(ID + "Changing group volume.");
	float previousGroupVolume = -1.0f;
	float currentGroupVolume = -1.0f;
	BOOST_CHECK_NO_THROW(previousGroupVolume = sound->getGroupVolume(ISound::ChannelGroup::MASTER));
	BOOST_CHECK_NO_THROW(sound->setGroupVolume(ISound::ChannelGroup::MASTER, 0.5f));
	BOOST_CHECK_NO_THROW(currentGroupVolume = sound->getGroupVolume(ISound::ChannelGroup::MASTER));
	BOOST_CHECK_MESSAGE(previousGroupVolume != currentGroupVolume && currentGroupVolume == 0.5f, ID + "The volume change failed.");

	BOOST_MESSAGE( ID + "Trying to pass non-valid number 2 and -1 to group volume change functions.");
	BOOST_CHECK_NO_THROW(sound->setGroupVolume(ISound::ChannelGroup::MASTER,-1.0f));
	BOOST_CHECK_NO_THROW(sound->setGroupVolume(ISound::ChannelGroup::MASTER, 2.0f));
	// ############### STEP 4 END ###############

	// ############### STEP 5 START ###############
	BOOST_MESSAGE(ID + "Step 5: Ininializing the Event Manager.");
	EventManager *eventManager = new EventManager();
	BOOST_CHECK_MESSAGE(eventManager != nullptr, ID + "The Event Manager pointer equals null. Creation failed.");
	BOOST_MESSAGE(ID + "Adding listeners for events.");
	eventManager->addListener(EventListenerDelegate(&SoundEngine::eventPlay), SoundEventIDataPlay::sk_EventType);
	eventManager->addListener(EventListenerDelegate(&SoundEngine::eventPause), SoundEventIDataPause::sk_EventType);
	// ############### STEP 5 END ###############

	// ############### STEP 6 START ###############
	BOOST_MESSAGE(ID + "Step 6: Using the sound engine with events.");

	BOOST_MESSAGE(ID + "Sending play sound event 'Default sound'");
	std::shared_ptr<SoundEventIDataPlay> playSound(new SoundEventIDataPlay(ts));
	BOOST_CHECK_NO_THROW(eventManager->triggerTriggerEvent(playSound));

	BOOST_MESSAGE(ID + "Sending pause sound event 'Default sound'");
	std::shared_ptr<SoundEventIDataPause> pauseSound(new SoundEventIDataPause(ts, true));
	BOOST_CHECK_NO_THROW(eventManager->triggerTriggerEvent(pauseSound));

	BOOST_MESSAGE(ID + "Sending unpause sound event 'Default sound'");
	std::shared_ptr<SoundEventIDataPause> unpauseSound(new SoundEventIDataPause(ts, false));
	BOOST_CHECK_NO_THROW(eventManager->queueEvent(unpauseSound));
	std::chrono::milliseconds processTime(10);
	BOOST_CHECK_MESSAGE(eventManager->processEvents(processTime) == true, ID + "Failed to process queued events.");
	// ############### STEP 6 END ###############

	// ############### STEP 7 START ###############
	BOOST_MESSAGE(ID + "Step 7: Clean up.");
	BOOST_MESSAGE(ID + "Starting to clean up the Event Manager.");
	if(eventManager)
	{
		delete eventManager;
		eventManager = nullptr;
	}
	BOOST_CHECK_MESSAGE(eventManager == nullptr, ID + "The Event Manager did not get cleaned up.");

	BOOST_MESSAGE(ID + "Starting to clean up the Sound engine.");
	BOOST_CHECK_NO_THROW(ISound::deleteSound(sound));
	// ############### STEP 7 END ###############

	BOOST_MESSAGE(ID + "Sound engine integration test completed.");
	BOOST_MESSAGE("");
}

BOOST_AUTO_TEST_SUITE_END()