#include "MemoryInformation.h"

MemoryInformation::MemoryInformation(void)
{
}

MemoryInformation::~MemoryInformation(void)
{
}

int MemoryInformation::getVirtualMemoryUsage(void)
{
	return m_VirtualMemUsage;
}

int MemoryInformation::getPhysicalMemoryUsage(void)
{
	return m_PhysicalMemUsage;
}

void MemoryInformation::update(void)
{
	HANDLE processHandle;

	processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, GetCurrentProcessId());
	if(!processHandle)
		return;

	m_ProcessInformation.cb = sizeof(m_ProcessInformation);
	if(GetProcessMemoryInfo(processHandle, (PROCESS_MEMORY_COUNTERS*)&m_ProcessInformation, m_ProcessInformation.cb))
	{
		m_VirtualMemUsage = m_ProcessInformation.PrivateUsage / MB;
		m_PhysicalMemUsage = m_ProcessInformation.WorkingSetSize / MB;
	}

	CloseHandle(processHandle);
}
