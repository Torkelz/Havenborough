#pragma once
#include <d3d11.h>

class Buffer
{
public:
	enum class Type
	{
		VERTEX_BUFFER,
		INDEX_BUFFER,
		CONSTANT_BUFFER_VS,
		CONSTANT_BUFFER_GS,
		CONSTANT_BUFFER_PS,
		BUFFER_TYPE_COUNT,
		CONSTANT_BUFFER_ALL,
		STAGING_BUFFER,
		STRUCTURED_BUFFER
	};

	enum class Usage
	{
		DEFAULT,
		STREAM_OUT_TARGET,
		CPU_WRITE,
		CPU_WRITE_DISCARD,
		CPU_READ,
		USAGE_COUNT,
		USAGE_IMMUTABLE,
		STAGING
	};

	struct Description
	{
		Type type;
		Usage usage;
		UINT32 numOfElements;
		UINT32 sizeOfElement;
		const void *initData;
		bool bindSRV;
		bool bindUAV;

		Description()
		{
			initData = nullptr;
			bindSRV = false;
			bindUAV = false;
		}
	};

private:
	ID3D11Buffer *m_Buffer;
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;
	D3D11_MAPPED_SUBRESOURCE m_MappedResource;

	UINT32 m_SizeOfElement;
	UINT32 m_NumOfElements;

	Type m_Type;
	Usage m_Usage;

	
public:
	Buffer(void);
	virtual ~Buffer(void);

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
	HRESULT initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, Description &p_Description);
	
	/**
	* Sets the the buffer to be applied.
	* @param p_StartSlot where to start in the buffer
	* @return S_OK if setting buffer, otherwise S_FALSE
	*/
	HRESULT setBuffer(UINT32 p_StartSlot);

	/**
	* Unsets the the buffer.
	* @param p_StartSlot where to start in the buffer
	* @return S_OK if setting buffer, otherwise S_FALSE
	*/
	HRESULT unsetBuffer(UINT32 p_StartSlot);

	/**
	* Maps a buffer usage to corresponding context.
	* @return pointer to mapped data
	*/
	void *map(void);
	
	/**
	* Unmaps the buffer pointer.
	*/
	void unmap(void);

	ID3D11ShaderResourceView* CreateBufferSRV(ID3D11Buffer* pBuffer);

private:
	void *mapResourceToContext(UINT32 p_MapType);

protected:
	virtual HRESULT createBuffer(D3D11_BUFFER_DESC *p_BufferDescription,
		D3D11_SUBRESOURCE_DATA *p_Data, ID3D11Buffer **p_Buffer);
};