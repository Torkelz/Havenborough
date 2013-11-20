#include "Buffer.h"

Buffer::Buffer(void)
{
	m_Buffer = nullptr;
	m_Device = nullptr;
	m_DeviceContext = nullptr;
}

Buffer::~Buffer(void)
{
}

ID3D11Buffer *Buffer::getBufferPointer(void) const
{
	return m_Buffer;
}

void Buffer::setBufferPointer(ID3D11Buffer *p_Buffer)
{
	m_Buffer = p_Buffer;
}

UINT32 Buffer::getSizeOfElement(void) const
{
	return m_NumOfElements;
}

UINT32 Buffer::getNumOfElements(void) const
{
	return m_NumOfElements;
}

void *Buffer::map(void)
{
	void *result = nullptr;
	UINT32 mapType = 0;

	switch (m_Usage)
	{
	case BUFFER_DEFAULT:
		break;
	case BUFFER_STREAM_OUT_TARGET:
		break;
	case BUFFER_CPU_WRITE:
		{
			mapType = BUFFER_CPU_WRITE;
			mapResourceToContext(mapType);
			break;
		}
		
	case BUFFER_CPU_WRITE_DISCARD:
		{
			mapType = BUFFER_CPU_WRITE_DISCARD;
			mapResourceToContext(mapType);
			break;
		}
	case BUFFER_CPU_READ:
		{
			mapType = BUFFER_CPU_READ;
			mapResourceToContext(mapType);
			break;
		}
		
	case BUFFER_USAGE_COUNT:
		break;
	case BUFFER_USAGE_IMMUTABLE:
		break;
	default:
		break;
	}

	return result;
}

void Buffer::unmap(void)
{
	m_DeviceContext->Unmap(m_Buffer, 0);
}

void *Buffer::mapResourceToContext(UINT32 p_MapType)
{
	if(FAILED(m_DeviceContext->Map(m_Buffer, 0, (D3D11_MAP)p_MapType, 0, &m_MappedResource)))
	{
		return nullptr;
	}
	else
	{
		return m_MappedResource.pData;
	}
}