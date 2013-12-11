#include "Buffer.h"
#include "VRAMMemInfo.h"

Buffer::Buffer(void)
{
	m_Buffer = nullptr;
	m_Device = nullptr;
	m_DeviceContext = nullptr;
}

Buffer::~Buffer(void)
{
	VRAMMemInfo::getInstance()->updateUsage(-(int)(m_SizeOfElement * m_NumOfElements));
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
	Description &p_Description)
{
	HRESULT result = S_OK;
	D3D11_BUFFER_DESC bufferDescription;

	m_Device = p_Device;
	m_DeviceContext = p_DeviceContext;
	m_Type = p_Description.type;

	bufferDescription.StructureByteStride = 0;
	bufferDescription.MiscFlags = 0;
	
	switch (m_Type)
	{
	case Type::VERTEX_BUFFER:
		{
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			if(p_Description.usage == Usage::STREAM_OUT_TARGET)
			{
				bufferDescription.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
			}
			break;
		}
	case Type::INDEX_BUFFER:
		{
			bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			break;
		}
	case Type::CONSTANT_BUFFER_VS:
	case Type::CONSTANT_BUFFER_GS:
	case Type::CONSTANT_BUFFER_PS:
	case Type::BUFFER_TYPE_COUNT:
	case Type::CONSTANT_BUFFER_ALL:
		{
			bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			break;
		}
	case  Type::STAGING_BUFFER:
		{
			bufferDescription.BindFlags = 0;
			break;
		}

	case Type::STRUCTURED_BUFFER:
		{
			bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bufferDescription.StructureByteStride = p_Description.sizeOfElement;
			bufferDescription.BindFlags = 0;
			break;
		}

	default:
		{
			return S_FALSE;
			break;
		}
	}

	if(p_Description.bindSRV)	bufferDescription.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if(p_Description.bindUAV)	bufferDescription.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	m_Usage = p_Description.usage;
	m_SizeOfElement = p_Description.sizeOfElement;
	m_NumOfElements = p_Description.numOfElements;
	bufferDescription.CPUAccessFlags = 0;
	switch (m_Usage)
	{
	case Usage::DEFAULT:
		{
			bufferDescription.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
	case Usage::STREAM_OUT_TARGET:
		{
			bufferDescription.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
	case Usage::CPU_WRITE:
		{
			bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
			bufferDescription.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
			break;
		}
	case Usage::CPU_WRITE_DISCARD:
		{
			bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
			bufferDescription.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
			break;
		}
	case Usage::CPU_READ:
		{
			if(m_Type != Type::STAGING_BUFFER)
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
	case Usage::USAGE_COUNT:
		{
			bufferDescription.Usage = D3D11_USAGE_DEFAULT;
			break;
		}
	case Usage::USAGE_IMMUTABLE:
		{
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			break;
		}
	default:
		{
			break;
		}
	}

	
	/*if(m_Type ==Type::STRUCTURED_BUFFER)
	{
		bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}*/

	bufferDescription.ByteWidth = p_Description.numOfElements * p_Description.sizeOfElement;

	////Make sure at least 16 bytes is set.
	//if(bufferDescription.ByteWidth < 16)
	//{
	//	bufferDescription.ByteWidth = 16;
	//}
	//set at least 16 bytes
	bufferDescription.ByteWidth = ((bufferDescription.ByteWidth + 15) / 16) * 16;

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
	case Type::VERTEX_BUFFER:
		{
			UINT32 offset = 0;
			UINT32 numOfBuffers = 1;
			m_DeviceContext->IASetVertexBuffers(p_StartSlot, numOfBuffers, &m_Buffer, &m_SizeOfElement, &offset);
			break;
		}
	case Type::INDEX_BUFFER:
		{
			UINT32 offset = 0;
			m_DeviceContext->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, offset);
			break;
		}
	case Type::CONSTANT_BUFFER_VS:
		{
			UINT32 numOfBuffers = 1;
			m_DeviceContext->VSSetConstantBuffers(p_StartSlot, numOfBuffers, &m_Buffer);
			break;
		}
	case Type::CONSTANT_BUFFER_GS:
		{
			UINT32 numOfBuffers = 1;
			m_DeviceContext->GSSetConstantBuffers(p_StartSlot, numOfBuffers, &m_Buffer);
			break;
		}
	case Type::CONSTANT_BUFFER_PS:
		{
			UINT32 numOfBuffers = 1;
			m_DeviceContext->PSSetConstantBuffers(p_StartSlot, numOfBuffers, &m_Buffer);
			break;
		}
	case Type::BUFFER_TYPE_COUNT:
		{
			break;

		}
	case Type::CONSTANT_BUFFER_ALL:
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

HRESULT Buffer::unsetBuffer(UINT32 p_StartSlot)
{
	HRESULT result = S_OK;

	switch(m_Type)
	{
	case Type::VERTEX_BUFFER:
		{
			UINT32 offset = 0;
			m_DeviceContext->IASetVertexBuffers(p_StartSlot, 0, nullptr, 0, &offset);
			break;
		}
	case Type::INDEX_BUFFER:
		{
			UINT32 offset = 0;
			m_DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, offset);
			break;
		}
	case Type::CONSTANT_BUFFER_VS:
		{
			m_DeviceContext->VSSetConstantBuffers(p_StartSlot, 0, nullptr);
			break;
		}
	case Type::CONSTANT_BUFFER_GS:
		{
			m_DeviceContext->GSSetConstantBuffers(p_StartSlot, 0, nullptr);
			break;
		}
	case Type::CONSTANT_BUFFER_PS:
		{
			m_DeviceContext->PSSetConstantBuffers(p_StartSlot, 0, nullptr);
			break;
		}
	case Type::BUFFER_TYPE_COUNT:
		{
			break;

		}
	case Type::CONSTANT_BUFFER_ALL:
		{
			m_DeviceContext->VSSetConstantBuffers(p_StartSlot, 0, nullptr);
			m_DeviceContext->GSSetConstantBuffers(p_StartSlot, 0, nullptr);
			m_DeviceContext->PSSetConstantBuffers(p_StartSlot, 0, nullptr);
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
	case Usage::DEFAULT:
		break;
	case Usage::STREAM_OUT_TARGET:
		break;
	case Usage::CPU_WRITE:
		{
			mapType = (UINT32)Usage::CPU_WRITE;
			result = mapResourceToContext(mapType);
			break;
		}
		
	case Usage::CPU_WRITE_DISCARD:
		{
			mapType = (UINT32)Usage::CPU_WRITE_DISCARD;
			result = mapResourceToContext(mapType);
			break;
		}
	case Usage::CPU_READ:
		{
			mapType = (UINT32)Usage::CPU_READ;
			result = mapResourceToContext(mapType);
			break;
		}
	case Usage::USAGE_COUNT:
		break;
	case Usage::USAGE_IMMUTABLE:
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

ID3D11ShaderResourceView* Buffer::CreateBufferSRV(ID3D11Buffer* pBuffer)
{
	ID3D11ShaderResourceView* pSRVOut = NULL;

    D3D11_BUFFER_DESC descBuf;
    ZeroMemory(&descBuf, sizeof(descBuf));
    pBuffer->GetDesc(&descBuf);

    D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    desc.BufferEx.FirstElement = 0;

    if(descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
    {
        // This is a Raw Buffer
        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
        desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
    }
        else if(descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
    {
        // This is a Structured Buffer
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
    }
	else
	{
		return NULL;
	}

    m_Device->CreateShaderResourceView(pBuffer, &desc, &pSRVOut);

	return pSRVOut;
}