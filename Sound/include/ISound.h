#pragma once

#include <cstdint>

class ISound
{
public:

	/**
	 * Destructor
	 */
	__declspec(dllexport) static ISound *createSound(void);
	/**
	 * Destructor
	 */
	__declspec(dllexport) static void deleteSound(ISound *p_Sound);
    /**
	 * Destructor
	 */
	virtual ~ISound(void) {}
	/**
	 * Destructor
	 */
	virtual void initialize(void) = 0;
	/**
	 * Destructor
	 */
	virtual bool loadSound(const char *p_Filename) = 0;
	/**
	 * Destructor
	 */
	virtual void playSound(void) = 0;
	/**
	 * Destructor
	 */
	virtual bool releaseSound(void) = 0;
	/**
	 * Destructor
	 */
	virtual void shutdown(void) = 0;
private:

};