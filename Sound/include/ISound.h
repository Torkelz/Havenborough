#pragma once

#include <cstdint>
#include <Utilities\Util.h>

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
	 * Updates the FMOD listener system, should be called every frame.
	 *
	 * @param p_Position the listeners position
	 * @param p_Velocity the velocity of the listener
	 * @param p_Forward listeners look direction
	 * @param p_Up vector for the listener
	 */
	virtual void onFrameListener(Vector3* p_Position, Vector3* p_Velocity, Vector3* p_Forward, Vector3* p_Up)  = 0;

	/**
	 * Updates the FMOD sound system, should be called every frame.
	 *
	 * @param p_SoundID the sound id
	 * @param p_Position the listeners position
	 * @param p_Velocity the velocity of the listener
	 */
	virtual void onFrameSound(int p_SoundID, Vector3* p_Position, Vector3* p_Velocity) = 0;

	/**
	 * Updates the FMOD system, should be called every frame.
	 */
	virtual void onFrame(void) = 0;
	/**
	 * Load a sound from file into memory and set the loop state to be on.
	 * @param p_SoundId resource name connected to the sound to load.
	 * @param p_Filename filename of the sound file.
	 * @param true if the load is successful otherwise false
	 */
	virtual bool loadSound(const char *p_SoundId, const char *p_Filename) = 0;

	virtual void set3DMinDistance(int p_SoundId, float p_MinDistance) = 0;

	virtual void setSoundModes(int p_SoundId, bool p_3D, bool p_Loop) = 0;

	virtual int createSoundInstance(const char *p_SoundId) = 0;

	/**
	 * A way find out if an instance of a sound is playing.
	 * @param p_SoundId resource name connected to the sound to load.
	 * @return true if it's still playing else false
	 * 
	 */
	virtual bool isPlaying(int p_SoundId) = 0;
	/**
	 * Stream a sound from file, NOT IMPLEMENTED!
	 * @param p_SoundId resource name connected to the sound to load.
	 * @param p_Filename filename of the sound file.
	 * @param true if the load is successful otherwise false
	 */
	virtual bool loadStream(const char *p_SoundId, const char *p_Filename) = 0;
	/**
	 * Play a sound.
	 * @param p_SoundId which sound to play
	 */
	virtual void playSound(int p_SoundId) = 0;

	/**
	 * Play 3D sound from a position with a velocity;
	 * @param p_SoundId wich sound to play
	 * @param the sounds start position
	 * @param the sounds velocity
	 */
	virtual void play3DSound(int p_SoundId, Vector3* p_Position, Vector3* p_Velocity) = 0;

	/**
	 * Pause/unpause a sound.
	 * @param p_SoundId which sound to pause
	 * @param p_Pause true pauses the sound, false unpauses the sound
	 */
	virtual void pauseSound(int p_SoundId, bool p_Pause) = 0;
	/**
	 * Stop a sound.
	 * @param p_SoundId which sound to stop
	 */
	virtual void stopSound(int p_SoundId) = 0;
	/**
	 * Add a sound to a group.
	 * @param p_SoundId which sound to add
	 * @param p_Group which group to add the sound to.
	 */
	virtual void addSoundToGroup(int SoundId, ChannelGroup p_Group) = 0;
	/**
	 * Add a sound to a group.
	 * @param p_SoundId which sound to remove
	 * @param p_Group which group to remove the sound from.
	 */
	/*virtual void RemoveSoundFromGroup(const char *SoundId, ChannelGroup p_Group) = 0;*/
	/**
	 * Set Volume of a specific sound.
	 * @param p_SoundId which channel to change the volume on
	 * @param p_Volume the new volume value which should 0.0 - 1.0 where 0.0 is mute and 1.0 is max volume.
	 */
	virtual void setSoundVolume(int p_SoundId, float p_Volume) = 0;
	/**
	 * Set Volume for all sounds in a group.
	 * @param p_Group which group to change the Volume on, use Master to change volume all sounds.
	 * @param p_Volume the new volume value which should be 0.0 - 1.0 where 0.0 is mute and 1.0 is max volume.
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

	/**
	 * Get functions for debug and/or volume bar on the UI.
	 *
	 * @param identification for the sound/group to get volume from.
	 */
	virtual float getVolume(int p_SoundId) = 0;
	virtual float getGroupVolume(ISound::ChannelGroup p_Group) = 0;

	/**
	 * Get functions for debug and/or pause indicator on the UI.
	 *
	 * @param identification for the sound/group to get paused status from.
	 */
	virtual bool getPaused(int p_SoundId) = 0;
	/**
	 * Get functions for debug and/or mute indicator on the UI.
	 *
	 * @param identification for the sound/group to get paused status from.
	 */
	virtual bool getMasterMute() = 0;
	
private:

};