#include "Buffer.h"

Buffer::Buffer(void)
{
}

Buffer::~Buffer(void)
{
}

void *Buffer::map(void)
{
	void *result = nullptr;
	switch(m_Usage)
	{
	case BUFFER_DEFAULT:
		break;
	case BUFFER_STREAM_OUT_TARGET:
		break;
	case BUFFER_CPU_WRITE:
		break;
	case BUFFER_CPU_WRITE_DISCARD:
		break;
	case BUFFER_CPU_READ:
		break;
	case BUFFER_USAGE_COUNT:
		break;
	case BUFFER_USAGE_IMMUTABLE:
		break;
	default:
		break;
	}
}

void Buffer::unmap(void)
{
	m_DeviceContext->Unmap(m_Buffer, 0);
}