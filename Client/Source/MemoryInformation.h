#pragma once
#define WIN32_LEAN_AND_MEAN
#define KB 1024
#define MB (1024 * KB)

#include <windows.h>
#include <stdio.h>
#include <Psapi.h>


class MemoryInformation
{
private:
	PROCESS_MEMORY_COUNTERS_EX m_ProcessInformation;
	SIZE_T m_VirtualMemUsage;
	SIZE_T m_PhysicalMemUsage;

public:
	MemoryInformation(void);
	~MemoryInformation(void);

	int getVirtualMemoryUsage(void);
	int getPhysicalMemoryUsage(void);

	void update(void);
};