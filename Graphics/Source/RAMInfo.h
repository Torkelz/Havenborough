#pragma once
#define KB 1000
#define MB (1000 * KB)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Psapi.h>

class RAMInfo
{
private:
	PROCESS_MEMORY_COUNTERS_EX m_ProcessInformation;
	SIZE_T m_VirtualMemUsage;
	SIZE_T m_PhysicalMemUsage;
	
public:
	/**
	* Constructor
	*/
	RAMInfo(void);
	
	/**
	* Destructor
	*/
	~RAMInfo(void);

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