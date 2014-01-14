#pragma once

#include "../include/ISound.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include <vector>

class Sound : public ISound
{
private:
	FMOD::System				*m_System;
	FMOD_RESULT					m_Result;
	FMOD_SPEAKERMODE			m_SpeakerMode;
	FMOD_CAPS					m_Caps;
	std::vector<FMOD::Sound>	m_Sounds;
	std::vector<FMOD::Channel>	m_Channels;
	int							m_Key;
	int							m_NumDrivers;
	unsigned int				m_Version;
	char						m_Name[256];


public:
	Sound(void);
	~Sound(void);

	void initialize(void) override;

	bool loadSound(const char *p_Filename) override;

	void playSound(void) override;

	bool releaseSound(void) override;

	void shutdown(void) override;

private:
	void errorCheck(FMOD_RESULT p_Result);
};