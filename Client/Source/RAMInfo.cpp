#include "RAMInfo.h"

RAMInfo::RAMInfo(void)
{
}

RAMInfo::~RAMInfo(void)
{
}

unsigned int RAMInfo::getVirtualMemoryUsage(void)
{
	return m_VirtualMemUsage;
}

unsigned int RAMInfo::getPhysicalMemoryUsage(void)
{
	return m_PhysicalMemUsage;
}

void RAMInfo::update(void)
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
