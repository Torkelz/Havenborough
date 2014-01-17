#include "Sound.h"

#include "SoundLogger.h"



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
	p_Sound = nullptr;
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
		errorCheck(m_System->init(100, FMOD_INIT_NORMAL, 0));
	}

	errorCheck(m_System->createChannelGroup("Music Group", &m_MusicChannelGroup));

	errorCheck(m_System->createChannelGroup("Sfx Group", &m_SfxChannelGroup));

	errorCheck(m_System->getMasterChannelGroup(&m_MasterChannelGroup));

	errorCheck(m_MasterChannelGroup->addGroup(m_MusicChannelGroup));

	errorCheck(m_MasterChannelGroup->addGroup(m_SfxChannelGroup));
}

void Sound::onFrame(void)
{
	m_System->update();
}

bool Sound::loadSound(const char *p_SoundId, const char *p_Filename)
{

	FMOD::Sound *s;
	FMOD::Channel *c;
	errorCheck(m_System->createSound(p_Filename, FMOD_LOOP_NORMAL, 0, &s));
	errorCheck(m_System->playSound(FMOD_CHANNEL_FREE, s, true, &c));

	SoundInstance si(p_SoundId, s, c);
	m_Sounds.push_back(si);
	s = nullptr;
	
	return true;
}

bool Sound::loadStream(const char *p_SoundId, const char *p_Filename)
{
	return true;
}

void Sound::playSound(const char *p_SoundId)
{
	SoundInstance *s = getSound(std::string(p_SoundId));
	FMOD::Channel *c;
	if(s->getChannel())
	{
		bool p;
		errorCheck(s->getChannel()->getPaused(&p));
		if(p)
			errorCheck(s->getChannel()->setPaused(false));
		else
		{
			errorCheck(m_System->playSound(FMOD_CHANNEL_FREE, s->getSound(), false, &c));
			s->setChannel(c);
		}
	}
	else
	{
		errorCheck(m_System->playSound(FMOD_CHANNEL_FREE, s->getSound(), false, &c));
		s->setChannel(c);
	}
}

void Sound::pauseSound(const char *p_SoundId, bool p_Pause)
{
	SoundInstance *s = getSound(std::string(p_SoundId));
	if(s->getChannel())
	{
		bool p;
		s->getChannel()->getPaused(&p);
		if(p != p_Pause)
		errorCheck(s->getChannel()->setPaused(p_Pause));
	}
}

void Sound::stopSound(const char *p_SoundId)
{
	SoundInstance *s = getSound(std::string(p_SoundId));
	if(s->getChannel())
	{
		bool p;
		s->getChannel()->getPaused(&p);
		if(!p)
		{
			errorCheck(s->getChannel()->stop()); 
			s->setChannel(nullptr);
		}
	}
}

void Sound::addSoundToGroup(const char* p_SoundId, ISound::ChannelGroup p_Group)
{
	SoundInstance *s = getSound(std::string(p_SoundId));
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

void Sound::setSoundVolume(const char *p_SoundId, float p_Volume)
{
	SoundInstance *s = getSound(std::string(p_SoundId));
	if(s->getChannel())
	{
		errorCheck(s->getChannel()->setVolume(p_Volume));
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
	for(auto it = m_Sounds.begin(); it != m_Sounds.end(); ++it)
	{
		if(strcmp(it->getSoundId().c_str(), p_SoundId) == 0)
		{
			errorCheck(it->destroy());
			m_Sounds.erase(it);
			return true;
		}
	}
	return false;
}

void Sound::shutdown(void)
{
	for(auto &s : m_Sounds)
	{
		releaseSound(s.getSoundId().c_str());
	}

	m_Sounds.clear();

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

SoundInstance *Sound::getSound(std::string p_SoundId)
{
	for(auto &s : m_Sounds)
	{
		if(s.getSoundId() == p_SoundId)
		{
			return &s;
		}
	}
	return nullptr;
}

