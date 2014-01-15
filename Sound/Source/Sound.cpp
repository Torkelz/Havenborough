#include "Sound.h"

#include "SoundLogger.h"

Sound::Sound(void)
{
	m_System = nullptr;
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
	m_Result = FMOD::System_Create(&m_System);	
	errorCheck();

	m_Result = m_System->getVersion(&m_Version);
	errorCheck();

	if (m_Version < FMOD_VERSION)
	{
		SoundLogger::log(SoundLogger::Level::ERROR_L, "FMOD Error! You are using an old version of FMOD v." + std::to_string(m_Version) + ", This game requires v." + std::to_string(FMOD_VERSION));
		//throw exception
	}

	m_Result = m_System->getNumDrivers(&m_NumDrivers);
	errorCheck();

	if (m_NumDrivers == 0)
	{
		m_Result = m_System->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		errorCheck();
	}
	else
	{
		m_Result = m_System->getDriverCaps(0, &m_Caps, 0, &m_SpeakerMode);
		errorCheck();

		/*
			Set the user selected speaker mode.
		*/
		m_Result = m_System->setSpeakerMode(m_SpeakerMode);
		errorCheck();


		if (m_Caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			/*
			The user has the 'Acceleration' slider set to off! This is really bad
			for latency! You might want to warn the user about this.
			*/
			m_Result = m_System->setDSPBufferSize(1024, 10);
			errorCheck();
		}

		m_Result = m_System->getDriverInfo(0, m_Name, 256, 0);
		errorCheck();

		if (strstr(m_Name, "SigmaTel"))
		{
			/*
			Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			PCM floating point output seems to solve it.
			*/
			m_Result = m_System->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0,
			FMOD_DSP_RESAMPLER_LINEAR);
			errorCheck();
		}
	}
	
	m_Result = m_System->init(100, FMOD_INIT_NORMAL, 0);
	if (m_Result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		/*
			Ok, the speaker mode selected isn't supported by this soundcard. Switch it
			back to stereo...
		*/
		m_Result = m_System->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		errorCheck();
		/*
			... and re-init.
		*/
		m_Result = m_System->init(100, FMOD_INIT_NORMAL, 0);
	}
	errorCheck();

	m_Result = m_System->createChannelGroup("Music Group", &m_MusicChannelGroup);
	errorCheck();

	m_Result = m_System->createChannelGroup("Sfx Group", &m_SfxChannelGroup);
	errorCheck();

	m_Result = m_System->getMasterChannelGroup(&m_MasterChannelGroup);
	errorCheck();

	m_Result = m_MasterChannelGroup->addGroup(m_MusicChannelGroup);
	errorCheck();

	m_Result = m_MasterChannelGroup->addGroup(m_SfxChannelGroup);
	errorCheck();
}

void Sound::onFrame(void)
{
	m_System->update();
}


bool Sound::loadSound(const char *p_SoundId, const char *p_Filename)
{

	FMOD::Sound *s;
	
	m_Result = m_System->createSound(p_Filename, FMOD_LOOP_OFF, 0, &s);
	errorCheck();
	m_Sounds.push_back(SoundInstance(p_SoundId, s));
	
	return true;
}

void Sound::playSound(const char *p_SoundId)
{
	for(auto &s : m_Sounds)
	{
		if(strcmp(s.getSoundId().c_str(), p_SoundId) == 0)
		{
			m_Result = m_System->playSound(FMOD_CHANNEL_FREE, s.getSound(), false, &s.getChannel()); 
			errorCheck();
			return;
		}
	}
}

void Sound::pauseSound(const char *p_SoundId)
{
	for(auto &s : m_Sounds)
	{
		if(strcmp(s.getSoundId().c_str(), p_SoundId) == 0)
		{
			m_Result = s.getChannel()->setPaused(true); 
			errorCheck();
			return;
		}
	}
}

void Sound::stopSound(const char *p_SoundId)
{
	
}

void Sound::setSoundVolume(const char *SoundId, float p_Volume)
{
	for(auto &s : m_Sounds)
	{
		if(strcmp(s.getSoundId().c_str(), SoundId) == 0)
		{
			if(s.getChannel())
			{
				m_Result = s.getChannel()->setVolume(p_Volume);
				errorCheck();
				break;
			}
		}
	}
}

void Sound::setMasterVolume(float p_Volume)
{
	m_MasterChannelGroup->setVolume(p_Volume);
}

void Sound::muteAll(void)
{
	m_Result = m_MasterChannelGroup->setMute(true);
	errorCheck();
}

void Sound::unmuteAll(void)
{
	m_Result = m_MasterChannelGroup->setMute(false);
	errorCheck();
}

bool Sound::releaseSound(const char *p_SoundId)
{
	for(auto it = m_Sounds.begin(); it != m_Sounds.end(); ++it)
	{
		if(strcmp(it->getSoundId.c_str(), p_SoundId) == 0)
		{
			m_Sounds.erase(it);
			errorCheck();
			return true;
		}
	}
	return false;
}

void Sound::shutdown(void)
{
	//for(auto &s : m_Sounds)
	//{
	//	m_Result = s.second->release();
	//	s.second = nullptr;
	//	errorCheck();
	//}

	m_Sounds.clear();
	errorCheck();

	m_Result = m_System->close();
	errorCheck();

	m_Result = m_System->release();
	m_System = nullptr;
	errorCheck();
}

void Sound::setLogFunction(clientLogCallback_t p_LogCallback)
{
	SoundLogger::setLogFunction(p_LogCallback);
}

void Sound::errorCheck()
{
	if(m_Result != FMOD_OK)
	{
		std::string str(FMOD_ErrorString(m_Result));
		SoundLogger::log(SoundLogger::Level::ERROR_L, "FMOD Error! " + str);
		//throw exception.
	}
}
