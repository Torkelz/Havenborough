#include "Sound.h"

#include "SoundLogger.h"

int Sound::m_NextHandle = 1;

int Sound::getNextHandle()
{
	return m_NextHandle++;
}
void Sound::resetSoundHandleCounter()
{
	m_NextHandle = 1;
}

Sound::Sound(void)
{
	m_System = nullptr;
	m_MasterChannelGroup = nullptr;
	m_MusicChannelGroup = nullptr;
	m_SfxChannelGroup = nullptr;
	m_Version = 0;
	m_Caps = 0;
	m_Key = 0;
	m_NumDrivers = 0;
	m_SpeakerMode = FMOD_SPEAKERMODE_STEREO;
}

Sound::~Sound(void)
{

}

ISound *ISound::createSound(void)
{
	return new Sound();
}

void ISound::deleteSound(ISound *p_Sound)
{
	SoundLogger::log(SoundLogger::Level::INFO, "Shutting down sound");
	p_Sound->shutdown();
	delete p_Sound;
}

void Sound::initialize(void)
{

	SoundLogger::log(SoundLogger::Level::INFO, "Initializing sound");
    //Create a System object and initialize
	errorCheck(FMOD::System_Create(&m_System));

	errorCheck(m_System->getVersion(&m_Version));
	
	if (m_Version < FMOD_VERSION)
	{
		SoundLogger::log(SoundLogger::Level::ERROR_L, "FMOD Error! You are using an old version of FMOD");
		throw SoundException("FMOD Error! You are using an old version of FMOD",__LINE__,__FILE__);
	}

	errorCheck(m_System->getNumDrivers(&m_NumDrivers));

	if (m_NumDrivers == 0)
	{
		errorCheck(m_System->setOutput(FMOD_OUTPUTTYPE_NOSOUND));
	}
	else
	{
		errorCheck(m_System->getDriverCaps(0, &m_Caps, 0, &m_SpeakerMode));

		/*
			Set the user selected speaker mode.
		*/
		errorCheck(m_System->setSpeakerMode(m_SpeakerMode));


		if (m_Caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			/*
			The user has the 'Acceleration' slider set to off! This is really bad
			for latency! You might want to warn the user about this.
			*/
			errorCheck(m_System->setDSPBufferSize(1024, 10));
		}

		errorCheck(m_System->getDriverInfo(0, m_Name, 256, 0));

		if (strstr(m_Name, "SigmaTel"))
		{
			/*
			Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			PCM floating point output seems to solve it.
			*/
			errorCheck(m_System->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0,
			FMOD_DSP_RESAMPLER_LINEAR));
		}
	}
	FMOD_RESULT r = m_System->init(100, FMOD_INIT_NORMAL, 0);
	errorCheck(r);
	if (r == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		/*
			Ok, the speaker mode selected isn't supported by this soundcard. Switch it
			back to stereo...
		*/
		errorCheck(m_System->setSpeakerMode(FMOD_SPEAKERMODE_STEREO));
		/*
			... and re-init.
		*/
		errorCheck(m_System->init(100, FMOD_INIT_NORMAL , 0));
	}
	errorCheck(m_System->set3DSettings(1.0f, 100.0f, 1.0f));

	errorCheck(m_System->createChannelGroup("Music Group", &m_MusicChannelGroup));

	errorCheck(m_System->createChannelGroup("Sfx Group", &m_SfxChannelGroup));

	errorCheck(m_System->getMasterChannelGroup(&m_MasterChannelGroup));

	errorCheck(m_MasterChannelGroup->addGroup(m_MusicChannelGroup));

	errorCheck(m_MasterChannelGroup->addGroup(m_SfxChannelGroup));

	
}

void Sound::onFrameListener(Vector3* p_Position, Vector3* p_Velocity, Vector3* p_Forward, Vector3* p_Up)
{
	m_System->set3DListenerAttributes(0,
		reinterpret_cast<FMOD_VECTOR*>(p_Position), 
		reinterpret_cast<FMOD_VECTOR*>(p_Velocity), 
		reinterpret_cast<FMOD_VECTOR*>(p_Forward), 
		reinterpret_cast<FMOD_VECTOR*>(p_Up));
}

void Sound::onFrameSound(int p_SoundID, Vector3* p_Position, Vector3* p_Velocity)
{
	SoundInstance* instSound = getSound(p_SoundID);
	instSound->getChannel()->set3DAttributes(
		reinterpret_cast<FMOD_VECTOR*>(p_Position) ,
		reinterpret_cast<FMOD_VECTOR*>(p_Velocity));
}

void Sound::onFrame(void)
{
	m_System->update();
}

bool Sound::loadSound(const char *p_SoundId, const char *p_Filename)
{

	FMOD::Sound *s;
	bool found = false;
	for (auto sound : m_SoundList)
	{
		if(strcmp( sound.first.c_str(), p_SoundId ) == 0)
		{
			found = true;
		}
	}
	if(!found)
	{
		errorCheck(m_System->createSound(p_Filename, FMOD_LOOP_NORMAL | FMOD_3D, 0, &s));
		m_SoundList.push_back(std::make_pair(std::string(p_SoundId), s));
	}
	return true;
}

int Sound::createSoundInstance(const char *p_SoundId)
{
	for (auto sound : m_SoundList)
	{
		if(strcmp( sound.first.c_str(), p_SoundId ) == 0)
		{
			SoundInstance si(sound.second);
			FMOD::Channel *c;
			int ID = getNextHandle();
			errorCheck(m_System->playSound(FMOD_CHANNEL_FREE, si.getSound(), true, &c));
			si.setChannel(c);
			m_InstanceList.push_back(std::make_pair(ID, si));
			return ID;
		}
	}
	throw SoundException("Sound not found!" + std::string(p_SoundId), __LINE__, __FILE__);
}

void Sound::set3DMinDistance(int p_SoundId, float minDistance)
{
	for(auto &instance : m_InstanceList)
	{
		if(instance.first == p_SoundId)
		{
			instance.second.getChannel()->set3DMinMaxDistance(minDistance, 10000.0f);
			return;
		}
	}
	throw SoundException("Sound not found!" + std::to_string(p_SoundId), __LINE__, __FILE__);
}

void Sound::setSoundModes(int p_SoundId, bool p_3D, bool p_Loop)
{
	SoundInstance *s = getSound(p_SoundId);
	if(!s->getSound())
		throw SoundException("Sound not found!"+ std::to_string(p_SoundId), __LINE__, __FILE__);

	FMOD_MODE m;
	errorCheck(s->getChannel()->getMode(&m));

	if(p_3D)
	{
		m |= FMOD_3D;
	}
	else
	{
		m &= ~FMOD_3D;
		m |= FMOD_2D;
	}
	if(p_Loop)
	{
		m |= FMOD_LOOP_NORMAL;
	}
	else
	{
		m &= ~FMOD_LOOP_NORMAL;
		m |= FMOD_LOOP_OFF;
	}
	errorCheck(s->getChannel()->setMode(m));
}

bool Sound::isPlaying(int p_SoundId)
{
	bool temp;
	SoundInstance *s = getSound(p_SoundId);
	errorCheck(s->getChannel()->isPlaying(&temp));

	return temp;
}

bool Sound::loadStream(const char *p_SoundId, const char *p_Filename)
{
	return true;
}

void Sound::playSound(int p_SoundId)
{
	SoundInstance *s = getSound(p_SoundId);
	FMOD::Channel *c;
	if(!s->getChannel())
	{
		errorCheck(m_System->playSound(FMOD_CHANNEL_FREE, s->getSound(), false, &c));
		s->setChannel(c);
	}
	else
	{
		bool p;
		errorCheck(s->getChannel()->getPaused(&p));
		if(p)
			errorCheck(s->getChannel()->setPaused(false));
	}
}

void Sound::play3DSound(int p_SoundId, Vector3* p_Position, Vector3* p_Velocity)
{
	SoundInstance *s = getSound(p_SoundId);
	FMOD::Channel *c;
	if(!s->getChannel())
	{
		errorCheck(m_System->playSound(FMOD_CHANNEL_FREE, s->getSound(), false, &c));

		errorCheck(c->set3DAttributes(
		reinterpret_cast<FMOD_VECTOR*>(p_Position) ,
		reinterpret_cast<FMOD_VECTOR*>(p_Velocity)));
		s->setChannel(c);
	}
	else
	{
		bool p;
		errorCheck(s->getChannel()->getPaused(&p));
		if(p)
			errorCheck(s->getChannel()->setPaused(false));

		errorCheck(s->getChannel()->set3DAttributes(
		reinterpret_cast<FMOD_VECTOR*>(p_Position) ,
		reinterpret_cast<FMOD_VECTOR*>(p_Velocity)));
		
	}
}

void Sound::pauseSound(int p_SoundId, bool p_Pause)
{
	SoundInstance *s = getSound(p_SoundId);
	if(s->getChannel())
	{
		bool p;
		errorCheck(s->getChannel()->getPaused(&p));
		if(p != p_Pause)
		errorCheck(s->getChannel()->setPaused(p_Pause));
	}
}

void Sound::stopSound(int p_SoundId)
{
	SoundInstance *s = getSound(p_SoundId);
	if(s->getChannel())
	{
		errorCheck(s->getChannel()->stop()); 
		s->setChannel(nullptr);
	}
}

void Sound::addSoundToGroup(int p_SoundId, ISound::ChannelGroup p_Group)
{
	SoundInstance *s = getSound(p_SoundId);
	if(s->getChannel())
	{
		switch(p_Group)
		{
		case ISound::ChannelGroup::MUSIC:
			s->getChannel()->setChannelGroup(m_MusicChannelGroup);
			return;
		case ISound::ChannelGroup::SFX:
			s->getChannel()->setChannelGroup(m_SfxChannelGroup);
			return;
		}
	}
}

void Sound::setGroupVolume(ISound::ChannelGroup p_Group, float p_Volume)
{
	switch(p_Group)
	{
	case ISound::ChannelGroup::MASTER:
		m_MasterChannelGroup->setVolume(p_Volume);
		return;
	case ISound::ChannelGroup::MUSIC:
		m_MusicChannelGroup->setVolume(p_Volume);
		return;
	case ISound::ChannelGroup::SFX:
		m_SfxChannelGroup->setVolume(p_Volume);
		return;
	}
}

void Sound::setSoundVolume(int p_SoundId, float p_Volume)
{
	SoundInstance *s = getSound(p_SoundId);
	if(s->getChannel())
	{
		errorCheck(s->getChannel()->setVolume(p_Volume));
	}
	else
	{
		throw SoundException("The sound channel does not exist.",__LINE__,__FILE__);
	}
}


void Sound::muteAll(bool m_Mute)
{
	errorCheck(m_MasterChannelGroup->setMute(m_Mute));
}

void Sound::pauseAll(bool p_Pause)
{
	errorCheck(m_MasterChannelGroup->setPaused(p_Pause));
}

bool Sound::releaseSound(const char *p_SoundId)
{
	for(auto it = m_SoundList.begin(); it != m_SoundList.end(); ++it)
	{
		if(strcmp(it->first.c_str(), p_SoundId) == 0)
		{
			errorCheck(it->second->release());
			m_SoundList.erase(it);
			return true;
		}
	}
	return false;
}

void Sound::shutdown(void)
{
	for(auto &s : m_SoundList)
	{
		errorCheck(s.second->release());
	}

	m_SoundList.clear();

	FMODSAFERELEASE(m_MusicChannelGroup);
	FMODSAFERELEASE(m_SfxChannelGroup);
	m_MasterChannelGroup = nullptr;

	errorCheck(m_System->close());

	FMODSAFERELEASE(m_System);
}

void Sound::setLogFunction(clientLogCallback_t p_LogCallback)
{
	SoundLogger::setLogFunction(p_LogCallback);
}

void Sound::errorCheck(FMOD_RESULT p_Result)
{
	if(p_Result != FMOD_OK)
	{
		SoundLogger::log(SoundLogger::Level::ERROR_L, "FMOD Error! " + std::string(FMOD_ErrorString(p_Result)));
		throw SoundException("FMOD Error!" + std::string(FMOD_ErrorString(p_Result)),__LINE__,__FILE__);
	}
}

SoundInstance *Sound::getSound(int p_SoundId)
{
	for(auto &s : m_InstanceList)
	{
		if(s.first == p_SoundId)
			return &s.second;
	}
	throw SoundException("Sound " + std::to_string(p_SoundId) + " not found. Is the file missing?", __LINE__, __FILE__);
}

float Sound::getVolume(int p_SoundId)
{
	float returnParam;
	errorCheck(getSound(p_SoundId)->getChannel()->getVolume( &returnParam ));
	return returnParam;
}

float Sound::getGroupVolume(ISound::ChannelGroup p_Group)
{
	float returnParam;
	switch(p_Group)
	{
	case ISound::ChannelGroup::MASTER:
		m_MasterChannelGroup->getVolume(&returnParam);
		break;
	case ISound::ChannelGroup::MUSIC:
		m_MusicChannelGroup->getVolume(&returnParam);
		break;
	case ISound::ChannelGroup::SFX:
		m_SfxChannelGroup->getVolume(&returnParam);
		break;
	}
	return returnParam;
}

bool Sound::getPaused(int p_SoundId)
{
	bool returnParam;
	errorCheck(getSound(p_SoundId)->getChannel()->getPaused(&returnParam));
	return returnParam;
}

bool Sound::getMasterMute()
{
	bool returnParam;
	errorCheck(m_MasterChannelGroup->getMute(&returnParam));
	return returnParam;
}