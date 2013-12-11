#include "RAMMemInfo.h"

RAMMemInfo::RAMMemInfo(void)
{
}

RAMMemInfo::~RAMMemInfo(void)
{
}

unsigned int RAMMemInfo::getVirtualMemoryUsage(void)
{
	return m_VirtualMemUsage;
}

unsigned int RAMMemInfo::getPhysicalMemoryUsage(void)
{
	return m_PhysicalMemUsage;
}

void RAMMemInfo::update(void)
{
	HANDLE processHandle;

	processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, GetCurrentProcessId());
	if(!processHandle)
		return;

	m_ProcessInformation.cb = sizeof(m_ProcessInformation);
	if(GetProcessMemoryInfo(processHandle, (PROCESS_MEMORY_COUNTERS*)&m_ProcessInformation, m_ProcessInformation.cb))
	{
		m_VirtualMemUsage = m_ProcessInformation.PrivateUsage / KB;
		m_PhysicalMemUsage = m_ProcessInformation.WorkingSetSize / KB;
	}

	CloseHandle(processHandle);
}
