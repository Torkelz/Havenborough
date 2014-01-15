#pragma once

#include "ISound.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include <vector>

namespace FMOD
{
	class System;
	class Channel;
}

class Sound : public ISound
{
private:
	struct SoundInstance
	{
		std::string		m_SoundId;
		FMOD::Sound		*m_Sound;
		FMOD::Channel	*m_Channel;

		explicit SoundInstance(std::string p_SoundId, FMOD::Sound *p_Sound)
			: m_SoundId(p_SoundId), m_Sound(p_Sound), m_Channel(nullptr)
		{
		}
		~SoundInstance()
		{
			m_Sound->release();
			m_Sound = nullptr;
			m_Channel = nullptr;
		}
		const std::string &getSoundId() const
		{
			return m_SoundId;
		}
		FMOD::Sound *getSound() const
		{
			return m_Sound;
		}
		FMOD::Channel *getChannel() 
		{
			return m_Channel;
		}
	};


	FMOD::System										*m_System;
	FMOD_RESULT											m_Result;
	FMOD_SPEAKERMODE									m_SpeakerMode;
	FMOD_CAPS											m_Caps;
	std::vector<SoundInstance>							m_Sounds;
	FMOD::ChannelGroup									*m_MusicChannelGroup;
	FMOD::ChannelGroup									*m_SfxChannelGroup;
	FMOD::ChannelGroup									*m_MasterChannelGroup;
	std::vector<std::pair<std::string, std::string>>	m_SoundChannelIDConnection;
	int													m_Key;
	int													m_NumDrivers;
	unsigned int										m_Version;
	char												m_Name[256];

public:
	Sound(void);
	~Sound(void);

	void initialize(void) override;

	void onFrame(void) override;

	bool loadSound(const char *p_SoundId, const char *p_Filename) override;

	void playSound(const char *p_SoundId) override;

	void pauseSound(const char *p_SoundId) override;

	void stopSound(const char *p_SoundId) override;

	void setSoundVolume(const char *p_SoundId, float p_Volume) override;

	void setMasterVolume(float p_Volume) override;

	void muteAll(void) override;

	void unmuteAll(void) override;
	
	bool releaseSound(const char *p_SoundId) override;

	void shutdown(void) override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;

private:
	void errorCheck();
};