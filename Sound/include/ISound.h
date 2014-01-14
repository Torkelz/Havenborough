#pragma once

class ISound
{
public:
	__declspec(dllexport) static ISound *createSound(void);
	__declspec(dllexport) static void deleteSound(ISound* p_Sound);


    /**
	 * Destructor
	 */
	virtual ~ISound(void) {};

	virtual void initialize(void) = 0;

	virtual bool loadSound(const char *p_Filename) = 0;

	virtual void playSound(void) = 0;

	virtual bool releaseSound(void) = 0;

	virtual void shutdown(void) = 0;
private:

};