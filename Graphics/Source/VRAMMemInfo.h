#pragma once
#define KB 1024
#define MB (1024 * KB)
#define OVERHEAD_USAGE (10 * MB) //This is a theoretical overhead for graphics resources in the video memory

#include "Util.h"

class VRAMMemInfo
{
private:
	static VRAMMemInfo *m_Instance;
	unsigned int m_Usage;

public:
	/**
	* Gets an instance of the VRAM memory info.
	* @return a pointer to the instance
	*/
	static VRAMMemInfo *getInstance(void);

	/**
	* Shuts down the info and release the memory allocated.
	*/
	void shutdown(void);

	/*
	* Gets the amount of video RAM the program uses.
	* @return the amount in MB
	*/
	int getUsage(void);

	/*
	* Updates the video RAM memory usage counter. Use positive value when storing something in memory, use negative 
	* when releasing something from memory.
	* @param p_Size the amount of usage in bytes
	*/
	void updateUsage(int p_Size);

private:
	VRAMMemInfo(void);
	~VRAMMemInfo(void);
};