#pragma once
#include <d3d11.h>

enum BufferType
{
	VERTEX_BUFFER,
	INDEX_BUFFER,
	CONSTANT_BUFFER_VS,
	CONSTANT_BUFFER_GS,
	CONSTANT_BUFFER_PS,
	BUFFER_TYPE_COUNT,
	CONSTANT_BUFFER_ALL
};

enum BufferUsage
{
	BUFFER_DEFAULT,
	BUFFER_STREAM_OUT_TARGET,
	BUFFER_CPU_WRITE,
	BUFFER_CPU_WRITE_DISCARD,
	BUFFER_CPU_READ,
	BUFFER_USAGE_COUNT,
	BUFFER_USAGE_IMMUTABLE
};

struct BufferDescription
{
	BufferType type;
	BufferUsage usage;
	UINT32 numOfElements;
	UINT32 sizeOfElement;
	void *initData;

	BufferDescription()
	{
		initData = nullptr;
	}
};

class Buffer
{
private:
	ID3D11Buffer *m_Buffer;
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;
	D3D11_MAPPED_SUBRESOURCE m_MappedResource;

	BufferType m_Type;
	BufferUsage m_Usage;
	
public:
	Buffer(void);
	~Buffer(void);

	void *map(void);
	void unmap(void);
};

