#include "VRAMMemInfo.h"

VRAMMemInfo *VRAMMemInfo::m_Instance = nullptr;

VRAMMemInfo *VRAMMemInfo::getInstance(void)
{
	if(!m_Instance)
		m_Instance = new VRAMMemInfo();

	return m_Instance;
}

void VRAMMemInfo::shutdown(void)
{
	SAFE_DELETE(m_Instance);
}

int VRAMMemInfo::getUsage(void)
{
	return m_Usage / MB;
}

void VRAMMemInfo::updateUsage(int size)
{
	m_Usage += size;
}

VRAMMemInfo::VRAMMemInfo(void)
{
	m_Usage = OVERHEAD_USAGE;
}

VRAMMemInfo::~VRAMMemInfo(void)
{
}