#include "Buffer.h"

Buffer::Buffer(void)
{
	m_Buffer = nullptr;
	m_Device = nullptr;
	m_DeviceContext = nullptr;
}

Buffer::~Buffer(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	SAFE_RELEASE(m_Buffer);
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

HRESULT Buffer::initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext,
	BufferDescription &p_Description)
{
	HRESULT result = S_OK;
	D3D11_BUFFER_DESC bufferDescription;

	m_Device = p_Device;
	m_DeviceContext = p_DeviceContext;
	m_Type = p_Description.type;

	bufferDescription.StructureByteStride = 0;
	
	switch (m_Type)
	{
	case VERTEX_BUFFER:
		{
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			if(p_Description.usage == BUFFER_STREAM_OUT_TARGET)
			{
				bufferDescription.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
			}
			break;
		}
	case INDEX_BUFFER:
		{
			bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			break;
		}
	case CONSTANT_BUFFER_VS:
	case CONSTANT_BUFFER_GS:
	case CONSTANT_BUFFER_PS:
	case BUFFER_TYPE_COUNT:
	case CONSTANT_BUFFER_ALL:
		{
			bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			break;
		}
	case  STAGING_BUFFER:
		{
			bufferDescription.BindFlags = 0;
			break;
		}

	default:
		{
			return S_FALSE;
			break;
		}
	}

	m_Usage = p_Description.usage;
	m_SizeOfElement = p_Description.sizeOfElement;
	m_NumOfElements = p_Description.numOfElements;
	bufferDescription.CPUAccessFlags = 0;
	switch (m_Usage)
	{
	case BUFFER_DEFAULT:
		{
			bufferDescription.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
	case BUFFER_STREAM_OUT_TARGET:
		{
			bufferDescription.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
	case BUFFER_CPU_WRITE:
		{
			bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
			bufferDescription.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
			break;
		}
	case BUFFER_CPU_WRITE_DISCARD:
		{
			bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
			bufferDescription.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
			break;
		}
	case BUFFER_CPU_READ:
		{
			if(m_Type != STAGING_BUFFER)
			{
				//m_DeviceContext = nullptr;
				//m_Device = nullptr;
				throw BufferException("Cannot set cpu read to other than staging buffer", __LINE__, __FILE__);
			}
			//bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
			bufferDescription.Usage = D3D11_USAGE_STAGING;
			bufferDescription.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
			break;
		}
	case BUFFER_USAGE_COUNT:
		{
			bufferDescription.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
	case BUFFER_USAGE_IMMUTABLE:
		{
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			break;
		}
	default:
		{
			break;
		}
	}

	bufferDescription.MiscFlags = 0;
	bufferDescription.ByteWidth = p_Description.numOfElements * p_Description.sizeOfElement;

	////Make sure at least 16 bytes is set.
	//if(bufferDescription.ByteWidth < 16)
	//{
	//	bufferDescription.ByteWidth = 16;
	//}
	//set at least 16 bytes
	bufferDescription.ByteWidth = bufferDescription.ByteWidth + (16 - bufferDescription.ByteWidth % 16);

	if(p_Description.initData)
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = p_Description.initData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		result = createBuffer(&bufferDescription, &data, &m_Buffer);
	}
	else
	{
		result = createBuffer(&bufferDescription, nullptr, &m_Buffer);
	}

	return result;
}

HRESULT Buffer::setBuffer(UINT32 p_StartSlot)
{
	HRESULT result = S_OK;

	switch(m_Type)
	{
	case VERTEX_BUFFER:
		{
			UINT32 offset = 0;
			UINT32 numOfBuffers = 1;
			m_DeviceContext->IASetVertexBuffers(p_StartSlot, numOfBuffers, &m_Buffer, &m_SizeOfElement, &offset);
			break;
		}
	case INDEX_BUFFER:
		{
			UINT32 offset = 0;
			m_DeviceContext->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, offset);
			break;
		}
	case CONSTANT_BUFFER_VS:
		{
			UINT32 numOfBuffers = 1;
			m_DeviceContext->VSSetConstantBuffers(p_StartSlot, numOfBuffers, &m_Buffer);
			break;
		}
	case CONSTANT_BUFFER_GS:
		{
			UINT32 numOfBuffers = 1;
			m_DeviceContext->GSSetConstantBuffers(p_StartSlot, numOfBuffers, &m_Buffer);
			break;
		}
	case CONSTANT_BUFFER_PS:
		{
			UINT32 numOfBuffers = 1;
			m_DeviceContext->PSSetConstantBuffers(p_StartSlot, numOfBuffers, &m_Buffer);
			break;
		}
	case BUFFER_TYPE_COUNT:
		{
			break;

		}
	case CONSTANT_BUFFER_ALL:
		{
			UINT32 numOfBuffers = 1;
			m_DeviceContext->VSSetConstantBuffers(p_StartSlot, numOfBuffers, &m_Buffer);
			m_DeviceContext->GSSetConstantBuffers(p_StartSlot, numOfBuffers, &m_Buffer);
			m_DeviceContext->PSSetConstantBuffers(p_StartSlot, numOfBuffers, &m_Buffer);
			break;
		}
	default:
		{
			result = S_FALSE;
			break;
		}
	}

	return result;
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
			result = mapResourceToContext(mapType);
			break;
		}
		
	case BUFFER_CPU_WRITE_DISCARD:
		{
			mapType = BUFFER_CPU_WRITE_DISCARD;
			result = mapResourceToContext(mapType);
			break;
		}
	case BUFFER_CPU_READ:
		{
			mapType = BUFFER_CPU_READ;
			result = mapResourceToContext(mapType);
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

HRESULT Buffer::createBuffer(D3D11_BUFFER_DESC *p_BufferDescription, D3D11_SUBRESOURCE_DATA *p_Data, ID3D11Buffer **p_Buffer)
{
	return m_Device->CreateBuffer(p_BufferDescription, p_Data, p_Buffer);
}