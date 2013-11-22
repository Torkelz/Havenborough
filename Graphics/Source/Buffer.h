#pragma once
#include <d3d11.h>

#include "Util.h"
#include "MyExceptions.h"


enum BufferType
{
	VERTEX_BUFFER,
	INDEX_BUFFER,
	CONSTANT_BUFFER_VS,
	CONSTANT_BUFFER_GS,
	CONSTANT_BUFFER_PS,
	BUFFER_TYPE_COUNT,
	CONSTANT_BUFFER_ALL,
	STAGING_BUFFER
};

enum BufferUsage
{
	BUFFER_DEFAULT,
	BUFFER_STREAM_OUT_TARGET,
	BUFFER_CPU_WRITE,
	BUFFER_CPU_WRITE_DISCARD,
	BUFFER_CPU_READ,
	BUFFER_USAGE_COUNT,
	BUFFER_USAGE_IMMUTABLE,
	BUFFER_STAGING
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

	UINT32 m_SizeOfElement;
	UINT32 m_NumOfElements;

	BufferType m_Type;
	BufferUsage m_Usage;

	
public:
	Buffer(void);
	~Buffer(void);

	/**
	* Gets hold of the pointer to the buffer.
	* @return the buffer pointer
	*/
	ID3D11Buffer *getBufferPointer(void) const;
	
	/**
	* Sets the pointer to the buffer.
	* @param p_Buffer the buffer pointer
	*/
	void setBufferPointer(ID3D11Buffer *p_Buffer);
	
	/**
	* Gets the size in bytes of one element in the buffer.
	* @return the size in bytes
	*/
	UINT32 getSizeOfElement(void) const;
	
	/**
	* Gets the number of elements in the buffer.
	* @return the number of elements.
	*/
	UINT32 getNumOfElements(void) const;

	/**
	* Initializes the buffer depending on the buffer type in the description.
	* @param p_Device pointer to the Direc3D device in use
	* @param p_DeviceContext pointer to the Direct3D device context in use
	* @param p_Description buffer description for the buffer to be initialized
	* @return S_OK if buffer initialized successfully
	*/
	HRESULT initialize(ID3D11Device *p_Device,
		ID3D11DeviceContext *p_DeviceContext, BufferDescription &p_Description);
	
	/**
	* 
	*/
	HRESULT setBuffer(UINT32 p_StartSlot);

	/**
	* 
	*/
	void *map(void);
	
	/**
	* 
	*/
	void unmap(void);

private:
	void *mapResourceToContext(UINT32 p_MapType);
};