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
	FMOD::Sound		*m_Sound;
	FMOD::Channel	*m_Channel;

	explicit SoundInstance(FMOD::Sound *p_Sound)
		: m_Sound(p_Sound)
	{
	}
	~SoundInstance()
	{
	}
	FMOD::Sound *getSound() const
	{
		return m_Sound;
	}
	void setChannel(FMOD::Channel *p_Channel)
	{
		m_Channel = p_Channel;
	}
	FMOD::Channel *getChannel()
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
	static int m_NextHandle;

	static int getNextHandle();

	FMOD::System										*m_System;
	FMOD_SPEAKERMODE									m_SpeakerMode;
	FMOD_CAPS											m_Caps;
	std::vector<std::pair<int, SoundInstance>>			m_InstanceList;
	FMOD::ChannelGroup									*m_MusicChannelGroup;
	FMOD::ChannelGroup									*m_SfxChannelGroup;
	FMOD::ChannelGroup									*m_MasterChannelGroup;
	std::vector<std::pair<std::string, FMOD::Sound*>>	m_SoundList; 
	int													m_Key;
	int													m_NumDrivers;
	unsigned int										m_Version;
	char												m_Name[256];

public:
	Sound(void);
	~Sound(void);

	void initialize(void) override;

	void onFrameListener(Vector3* p_Position, Vector3* p_Velocity, Vector3* p_Forward, Vector3* p_Up) override;

	void onFrameSound(int p_SoundID, Vector3* p_Position, Vector3* p_Velocity) override;

	void onFrame(void) override;

	bool loadSound(const char *p_SoundId, const char *p_Filename) override;

	void set3DMinDistance(int p_SoundId, float p_MinDistance) override;

	void setSoundModes(int p_SoundId, bool p_3D, bool p_Loop) override;

	int createSoundInstance(const char *p_SoundId) override;

	bool isPlaying(int p_SoundId) override;

	bool loadStream(const char *p_SoundId, const char *p_Filename) override;

	void playSound(int p_SoundId) override;

	void play3DSound(int p_SoundId, Vector3* p_Position, Vector3* p_Velocity) override;
	
	void pauseSound(int p_SoundId, bool p_Pause) override;

	void stopSound(int p_SoundId) override;

	void addSoundToGroup(int SoundId, ChannelGroup p_Group);

	void setGroupVolume(ISound::ChannelGroup p_Group, float p_Volume) override;
	
	void setSoundVolume(int p_SoundId, float p_Volume) override;

	void muteAll(bool p_Mute) override;

	void pauseAll(bool p_Pause) override;
	
	bool releaseSound(const char *p_SoundId) override;

	void shutdown(void) override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;

	float getVolume(int p_SoundId) override;

	float getGroupVolume(ISound::ChannelGroup p_Group) override;

	bool getPaused(int p_SoundId) override;

	bool getMasterMute() override;

	static void resetSoundHandleCounter();
private:
	void errorCheck(FMOD_RESULT p_Result);
	SoundInstance *getSound(int p_SoundId);
};