#pragma once

#include <cstdint>

class ISound
{
public:
	enum class ChannelGroup
	{
		MASTER,
		MUSIC,
		SFX
	};
	/**
	 * Create an instance of the sound library.
	 *
	 * Use {@link #deleteNetwork(ISound*)} to clean up.
	 *
	 * @return a pointer to the sound library implementation.
	 */
	__declspec(dllexport) static ISound *createSound(void);
	/**
	 * Clear sub resources allocated by the network and delete the library. 
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
	 * Set Volume of a specific sound.
	 * @param p_ChannelId which channel to change the volume on
	 * @param p_Volume the new volume value
	 */
	virtual void setSoundVolume(const char *p_ChannelId, float p_Volume) = 0;
	/**
	 * Set Volume for all sounds in a group.
	 * @param p_Group which group to change the Volume on, use Master to change volume all sounds.
	 * @param p_Volume the new volume value
	 */
	virtual void setGroupVolume(ChannelGroup p_Group, float p_Volume) = 0;
	/**
	 * Mute/unmute all sounds.
	 * @param p_Mute true = mutes all sounds, false = unmutes all sounds
	 */
	virtual void muteAll(bool p_Mute) = 0;
	/**
	 * Pause/unpause all sounds
	 * @p_Pause true = pauses all sounds, false = unpauses all sounds
	 */
	virtual void pauseAll(bool p_Pause) = 0;
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