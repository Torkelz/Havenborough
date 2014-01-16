#pragma once

#include "ISound.h"
#include "SoundExceptions.h"

#include <fmod.hpp>
#include <fmod_errors.h>
#include <vector>
#include <string>

#define FMODSAFERELEASE(x){ if(x){errorCheck(x->release()); x=nullptr;} }

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
	}
	const std::string &getSoundId() const
	{
		return m_SoundId;
	}
	FMOD::Sound *getSound() const
	{
		return m_Sound;
	}
	FMOD::Channel **getChannelRef()
	{
		return &m_Channel;
	}
	FMOD::Channel *getChannel() const
	{
		return m_Channel;
	}
	FMOD_RESULT destroy()
	{
		FMOD_RESULT r = m_Sound->release();
		m_Sound = nullptr;
		m_Channel = nullptr;
		return r;
	}
};

class Sound : public ISound
{
private:
	FMOD::System										*m_System;
	FMOD_SPEAKERMODE									m_SpeakerMode;
	FMOD_CAPS											m_Caps;
	std::vector<SoundInstance>							m_Sounds;
	FMOD::ChannelGroup									*m_MusicChannelGroup;
	FMOD::ChannelGroup									*m_SfxChannelGroup;
	FMOD::ChannelGroup									*m_MasterChannelGroup;
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

	void setGroupVolume(ISound::ChannelGroup p_Group, float p_Volume) override;
	
	void setSoundVolume(const char *p_SoundId, float p_Volume) override;

	void muteAll(bool p_Mute) override;

	void pauseAll(bool p_Pause) override;
	
	bool releaseSound(const char *p_SoundId) override;

	void shutdown(void) override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;

private:
	void errorCheck(FMOD_RESULT p_Result);
	SoundInstance *getSound(std::string p_SoundId);
};