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

	explicit SoundInstance(std::string p_SoundId, FMOD::Sound *p_Sound, FMOD::Channel *p_Channel)
		: m_SoundId(p_SoundId), m_Sound(p_Sound), m_Channel(p_Channel)
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
	void setChannel(FMOD::Channel *p_Channel)
	{
		m_Channel = p_Channel;
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

	void onFrameListener(Vector3* p_Position, Vector3* p_Velocity, Vector3* p_Forward, Vector3* p_Up) override;

	void onFrameSound(const char* p_SoundID, Vector3* p_Position, Vector3* p_Velocity) override;

	void onFrame(void) override;

	bool loadSound(const char *p_SoundId, const char *p_Filename) override;

	bool load3DSound(const char *p_SoundId, const char *p_Filename, float p_MinDistance) override;

	bool loadSoundWithoutLoop(const char *p_SoundId, const char *p_Filename) override;

	bool isPlaying(const char *p_SoundId) override;

	bool loadStream(const char *p_SoundId, const char *p_Filename) override;

	void playSound(const char *p_SoundId) override;

	void play3DSound(const char *p_SoundId, Vector3* p_Position, Vector3* p_Velocity) override;
	
	void pauseSound(const char *p_SoundId, bool p_Pause) override;

	void stopSound(const char *p_SoundId) override;

	void addSoundToGroup(const char *SoundId, ChannelGroup p_Group);

	void setGroupVolume(ISound::ChannelGroup p_Group, float p_Volume) override;
	
	void setSoundVolume(const char *p_SoundId, float p_Volume) override;

	void muteAll(bool p_Mute) override;

	void pauseAll(bool p_Pause) override;
	
	bool releaseSound(const char *p_SoundId) override;

	void shutdown(void) override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;

	float getVolume(const char* p_SoundId) override;

	float getGroupVolume(ISound::ChannelGroup p_Group) override;

	bool getPaused(const char* p_SoundId) override;

	bool getMasterMute() override;
private:
	void errorCheck(FMOD_RESULT p_Result);
	SoundInstance *getSound(std::string p_SoundId);
};