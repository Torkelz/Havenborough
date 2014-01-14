#include "Sound.h"


Sound::Sound(void)
{

}

Sound::~Sound(void)
{
	
}

ISound *ISound::createSound(void)
{
	return new Sound();
}

void Sound::initialize(void)
{
    //Create a System object and initialize
	m_Result = FMOD::System_Create(&m_System);
	errorCheck(m_Result);

	m_Result = m_System->getVersion(&m_Version);
	errorCheck(m_Result);

	if (m_Version < FMOD_VERSION)
	{
		//printf("Error! You are using an old version of FMOD %08x. This program requires %08x\n",
		//m_Version, FMOD_VERSION);
	}

	m_Result = m_System->getNumDrivers(&m_NumDrivers);
	errorCheck(m_Result);

	if (m_NumDrivers == 0)
	{
		m_Result = m_System->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		errorCheck(m_Result);
	}
	else
	{
		m_Result = m_System->getDriverCaps(0, &m_Caps, 0, &m_SpeakerMode);
		errorCheck(m_Result);

		/*
			Set the user selected speaker mode.
		*/
		m_Result = m_System->setSpeakerMode(m_SpeakerMode);
		errorCheck(m_Result);


		if (m_Caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			/*
			The user has the 'Acceleration' slider set to off! This is really bad
			for latency! You might want to warn the user about this.
			*/
			m_Result = m_System->setDSPBufferSize(1024, 10);
			errorCheck(m_Result);
		}

		m_Result = m_System->getDriverInfo(0, m_Name, 256, 0);
		errorCheck(m_Result);

		if (strstr(m_Name, "SigmaTel"))
		{
			/*
			Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			PCM floating point output seems to solve it.
			*/
			m_Result = m_System->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0,
			FMOD_DSP_RESAMPLER_LINEAR);
			errorCheck(m_Result);
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
		errorCheck(m_Result);
		/*
			... and re-init.
		*/
		m_Result = m_System->init(100, FMOD_INIT_NORMAL, 0);
	}
	errorCheck(m_Result);
}

bool Sound::loadSound(const char* p_Filename)
{
	return true;
}

void Sound::playSound(void)
{

}

bool Sound::releaseSound(void)
{
	return true;
}

void Sound::shutdown(void)
{
	for(auto &s : m_Sounds)
	{
		s.release();
	}
	m_Sounds.clear();
	m_Channels.clear();

	m_Result = m_System->close();
	errorCheck(m_Result);

	m_Result = m_System->release();
	errorCheck(m_Result);
}

void ISound::deleteSound(ISound *p_Sound)
{
	p_Sound->shutdown();
	delete p_Sound;
}

void Sound::errorCheck(FMOD_RESULT p_Result)
{
	if(p_Result != FMOD_OK)
	{
		//throw exception.
	}
}
