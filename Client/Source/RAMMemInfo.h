#pragma once
#define WIN32_LEAN_AND_MEAN
#define KB 1024
#define MB (1024 * KB)

#include <windows.h>
#include <stdio.h>
#include <Psapi.h>


class RAMMemInfo
{
private:
	PROCESS_MEMORY_COUNTERS_EX m_ProcessInformation;
	SIZE_T m_VirtualMemUsage;
	SIZE_T m_PhysicalMemUsage;
	
public:
	RAMMemInfo(void);
	~RAMMemInfo(void);

	/**
	* Gets the amount of physical RAM memory the program uses.
	* @return the amount in MB
	*/
	unsigned int getVirtualMemoryUsage(void);

	/**
	* Gets the amount of virtual RAM memory the program uses.
	* @return the amount in MB
	*/
	unsigned int getPhysicalMemoryUsage(void);

	/**
	* Updates the RAM memory usage counters.
	*/
	void update(void);
};