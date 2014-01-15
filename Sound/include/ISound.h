#pragma once

#include <cstdint>

class ISound
{
public:

	/**
	 * 
	 */
	__declspec(dllexport) static ISound *createSound(void);
	/**
	 * 
	 */
	__declspec(dllexport) static void deleteSound(ISound *p_Sound);
    /**
	 * Destructor
	 */
	virtual ~ISound(void) {}
	/**
	 * Initialize Sound and FMOD.
	 */
	virtual void initialize(void) = 0;
	/**
	 * Initialize Sound and FMOD.
	 */
	virtual void onFrame(void) = 0;
	/**
	 * Load a sound from file into memory.
	 * @param p_SoundId resource name connected to the sound to load.
	 * @param p_Filename filename of the sound file.
	 * @param true if the load is successful otherwise false
	 */
	virtual bool loadSound(const char *p_SoundId, const char *p_Filename) = 0;
	/**
	 * Play a sound.
	 * @param p_SoundId which sound to play
	 */
	virtual void playSound(const char *p_SoundId) = 0;
	/**
	 * Pause a sound.
	 * @param p_SoundId which sound to pause
	 */
	virtual void pauseSound(const char *p_SoundId) = 0;
	/**
	 * Stop a sound.
	 * @param p_SoundId which sound to stop
	 */
	virtual void stopSound(const char *p_SoundId) = 0;
	/**
	 * Set Volume of a specific channel.
	 * @param p_ChannelId which channel to change the volume on
	 * @param p_Volume the new volume value
	 */
	virtual void setSoundVolume(const char *p_ChannelId, float p_Volume) = 0;
	/**
	 * Set Volume for all channels.
	 * @param p_Volume the new volume value
	 */
	virtual void setMasterVolume(float p_Volume) = 0;
	/**
	 * Mute all channels.
	 */
	virtual void muteAll(void) = 0;
	/**
	 * Unmute all channels
	 */
	virtual void unmuteAll(void) = 0;
	/**
	 * Release a sound from memory
	 * @param p_SoundId which sound to release
	 * @return true if the release succeeded otherwise false
	 */
	virtual bool releaseSound(const char *p_SoundId) = 0;
	/**
	 * Shutdowns FMOD and releases all objects
	 */
	virtual void shutdown(void) = 0;
	/**
	 * Callback for logging.
	 *
	 * @param p_Level log priority level. Higher is more important
	 * @param p_Message the log message
	 */
	typedef void (*clientLogCallback_t)(uint32_t p_Level, const char* p_Message);
	/**
	 * Set the function to handle log messages.
	 *
	 * @param p_LogCallback the function to be called whenever a message is to
	 *			be logged from this component. Set to null to disable logging
	 */
	virtual void setLogFunction(clientLogCallback_t p_LogCallback) = 0;

private:

};