#include "VRAMInfo.h"
#include "GraphicsExceptions.h"

std::unique_ptr<VRAMInfo> VRAMInfo::m_Instance;

VRAMInfo *VRAMInfo::getInstance(void)
{
	if(!m_Instance)
		m_Instance.reset(new VRAMInfo());

	return m_Instance.get();
}

void VRAMInfo::shutdown(void)
{
	m_Instance.reset();
}

int VRAMInfo::getUsage(void) const
{
	return m_Usage;
}

void VRAMInfo::updateUsage(int size)
{
	m_Usage += size;
}

unsigned int VRAMInfo::calculateFormatUsage(DXGI_FORMAT p_Format, int p_Width, int p_Height)
{
	unsigned int result;

	switch (p_Format)
	{
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		{
			result = Size::R32G32B32A32_FLOAT * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_R32G32B32A32_UINT:
		{
			result = Size::R32G32B32A32_UINT * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_R32G32B32_FLOAT:
		{
			result = Size::R32G32B32_FLOAT * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_R32G32B32_TYPELESS:
		{
			result = Size::R32G32B32_TYPELESS * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_R32G32B32_UINT:
		{
			result = Size::R32G32B32_UINT * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		{
			result = Size::R16G16B16A16_FLOAT * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_R32G32_FLOAT:
		{
			result = Size::R32G32_FLOAT * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_R32G32_UINT:
		{
			result = Size::R32G32_UINT * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		{
			result = Size::R8G8B8A8_UNORM * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_R24G8_TYPELESS:
		{
			result = Size::R24G8_TYPELESS * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_R32_TYPELESS:
		{
			result = Size::R32_TYPELESS * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_BC3_UNORM:
		{
			result = Size::BC3_UNORM * p_Width * p_Height;
			break;
		}
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		{
			result = 0;
			break;
		}
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		{
			throw MemoryUsageException("Trying to load SRGB texture.", __LINE__, __FILE__);
			break;
		}
	default:
		{
			throw MemoryUsageException("Error when determining memory size of texture. Illegal file format or color type.", __LINE__, __FILE__);
			break;
		}
		
	}

	return result;
}

VRAMInfo::VRAMInfo(void)
{
	m_Usage = OVERHEAD_USAGE;
}

VRAMInfo::~VRAMInfo(void)
{
}