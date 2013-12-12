#define INITGUID
#include "Shader.h"
#include <vector>
#include <iostream>



using std::vector;

Shader::Shader(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_VertexShader = nullptr;
	m_GeometryShader = nullptr;
	m_PixelShader = nullptr;
	m_HullShader = nullptr;
	m_DomainShader = nullptr;
	m_VertexLayout = nullptr;
	m_VertexDescription = nullptr;
}

Shader::~Shader(void)
{
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_GeometryShader);
	SAFE_RELEASE(m_PixelShader);
	SAFE_RELEASE(m_HullShader);
	SAFE_RELEASE(m_DomainShader);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_DELETE_ARRAY(m_VertexDescription);
	m_Device = nullptr;
	m_DeviceContext = nullptr;
}

void Shader::initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, unsigned int p_NumOfElements)
{
	m_Device = p_Device;
	m_DeviceContext = p_DeviceContext;
	m_NumOfElements = p_NumOfElements;
}

HRESULT Shader::compileAndCreateShader(LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, Type p_ShaderType,
	const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout)
{
	HRESULT result = S_FALSE;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
	ID3DBlob *errorMessage = nullptr;
	ID3DBlob *shaderData = nullptr;


	result = compileShader(p_Filename, p_EntryPoint, p_ShaderModel, shaderFlags, shaderData, errorMessage);

	if(FAILED(result))
	{
		if(errorMessage == nullptr)
		{
			SAFE_RELEASE(errorMessage);
			SAFE_RELEASE(shaderData);
			std::wstring foo = p_Filename;
			throw ShaderException("Error when trying to compile shader. Could be missing file: " +
				std::string(foo.begin(), foo.end()), __LINE__, __FILE__);			
		}
		else
		{
			std::string temp = (std::string)(char*)errorMessage->GetBufferPointer();
			SAFE_RELEASE(errorMessage);
			throw ShaderException("Error when compiling shader.\n" + temp, __LINE__, __FILE__);
		}

		return result;
	}

	m_ShaderType = p_ShaderType;

	if(p_VertexLayout == nullptr && m_ShaderType == Type::VERTEX_SHADER)
	{
		createInputLayoutFromShaderSignature(shaderData);
	}
	else if(m_ShaderType == Type::VERTEX_SHADER)
	{
		m_VertexDescription = new D3D11_INPUT_ELEMENT_DESC[m_NumOfElements];
		std::copy(p_VertexLayout, p_VertexLayout + m_NumOfElements, m_VertexDescription);
	}

	releaseShader(p_ShaderType);
	result = createShader(shaderData);


	if(FAILED(result))
	{
		releaseShader(p_ShaderType);
		throw ShaderException("Error when creating shader.\n" + (std::string)(char*)errorMessage->GetBufferPointer(),
			__LINE__, __FILE__);
	}

	SAFE_RELEASE(shaderData);
	SAFE_RELEASE(errorMessage);

	return result;
}

void Shader::setShader(void)
{

	m_DeviceContext->IASetInputLayout(m_VertexLayout);
	m_DeviceContext->VSSetShader(m_VertexShader,0,0);

	m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);

	m_DeviceContext->GSSetShader(m_GeometryShader, nullptr, 0);

	m_DeviceContext->DSSetShader(m_DomainShader, nullptr, 0);

	m_DeviceContext->HSSetShader(m_HullShader, nullptr, 0);

}

void Shader::unSetShader(void)
{
	if(m_VertexShader)
	{
		m_DeviceContext->VSSetShader(nullptr, nullptr, 0);
	}
	if(m_PixelShader)
	{
		m_DeviceContext->PSSetShader(nullptr, nullptr, 0);
	}
	if(m_GeometryShader)
	{
		m_DeviceContext->GSSetShader(nullptr, nullptr, 0);
	}
	if(m_HullShader)
	{
		m_DeviceContext->HSSetShader(nullptr, nullptr, 0);
	}
	if(m_DomainShader)
	{
		m_DeviceContext->DSSetShader(nullptr, nullptr, 0);
	}
}

void Shader::setResource(Type p_ShaderType, UINT p_StartSpot, UINT p_NumOfViews,
	ID3D11ShaderResourceView *p_ShaderResource)
{
	switch(p_ShaderType)
	{
	case Type::VERTEX_SHADER:
		{
			m_DeviceContext->VSSetShaderResources(p_StartSpot, p_NumOfViews, &p_ShaderResource);
			break;
		}
	case Type::PIXEL_SHADER:
		{
			m_DeviceContext->PSSetShaderResources(p_StartSpot, p_NumOfViews, &p_ShaderResource);
			break;
		}
	case Type::GEOMETRY_SHADER:
		{
			m_DeviceContext->GSSetShaderResources(p_StartSpot, p_NumOfViews, &p_ShaderResource);
			break;
		}
	case Type::HULL_SHADER:
		{
			m_DeviceContext->HSSetShaderResources(p_StartSpot, p_NumOfViews, &p_ShaderResource);
			break;
		}
	case Type::DOMAIN_SHADER:
		{
			m_DeviceContext->DSSetShaderResources(p_StartSpot, p_NumOfViews, &p_ShaderResource);
			break;
		}
	}
}

void Shader::setSamplerState(Type p_ShaderType, UINT p_StartSpot, UINT p_NumOfSamples,
	ID3D11SamplerState *p_SamplerState)
{
	switch (p_ShaderType)
	{
	case Type::VERTEX_SHADER:
		{
			m_DeviceContext->VSSetSamplers(p_StartSpot, p_NumOfSamples, &p_SamplerState);
			break;
		}
	case Type::PIXEL_SHADER:
		{
			m_DeviceContext->PSSetSamplers(p_StartSpot, p_NumOfSamples, &p_SamplerState);
			break;
		}
	case Type::GEOMETRY_SHADER:
		{
			m_DeviceContext->GSSetSamplers(p_StartSpot, p_NumOfSamples, &p_SamplerState);
			break;
		}
	case Type::HULL_SHADER:
		{
			m_DeviceContext->HSSetSamplers(p_StartSpot, p_NumOfSamples, &p_SamplerState);
			break;
		}
	case Type::DOMAIN_SHADER:
		{
			m_DeviceContext->DSSetSamplers(p_StartSpot, p_NumOfSamples, &p_SamplerState);
			break;
		}
	default:
		{
			break;
		}
	}
}

void Shader::setBlendState(ID3D11BlendState *p_BlendState, float p_BlendFactor[4], UINT p_SampleMask)
{
	m_DeviceContext->OMSetBlendState(p_BlendState, p_BlendFactor, p_SampleMask);
}

void Shader::releaseShader(Shader::Type p_Type)
{
	switch(p_Type)
	{
	case Shader::Type::VERTEX_SHADER:
		SAFE_RELEASE(m_VertexShader);
		break;
	case Shader::Type::PIXEL_SHADER:
		SAFE_RELEASE(m_PixelShader);
		break;
	case Shader::Type::GEOMETRY_SHADER:
		SAFE_RELEASE(m_GeometryShader);
		break;
	case Shader::Type::HULL_SHADER:
		SAFE_RELEASE(m_HullShader);
		break;
	case Shader::Type::DOMAIN_SHADER:
		SAFE_RELEASE(m_DomainShader);
		break;
	default:
		break;
	}
}

void Shader::createInputLayoutFromShaderSignature(ID3DBlob *p_ShaderData)
{
	ID3D11ShaderReflection *shaderReflection = nullptr;
	D3D11_SHADER_DESC shaderDescription;
	UINT32 byteOffset;

	if(FAILED(D3DReflect(p_ShaderData->GetBufferPointer(), p_ShaderData->GetBufferSize(), IID_ID3D11ShaderReflection,
		(void**)&shaderReflection)))
	{
		throw ShaderException("Error when creating shader reflection", __LINE__, __FILE__);
	}

	shaderReflection->GetDesc(&shaderDescription);

	byteOffset = 0;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescription;
	for(UINT32 i = 0; i < shaderDescription.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC parameterDescription;
		shaderReflection->GetInputParameterDesc(i, &parameterDescription);

		D3D11_INPUT_ELEMENT_DESC elementDescription;   
		elementDescription.SemanticName = parameterDescription.SemanticName;      
		elementDescription.SemanticIndex = parameterDescription.SemanticIndex;
		elementDescription.InputSlot = 0;
		elementDescription.AlignedByteOffset = byteOffset;
		elementDescription.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDescription.InstanceDataStepRate = 0; 

		if(parameterDescription.Mask == 1)
		{
			if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				elementDescription.Format = DXGI_FORMAT_R32_UINT;
			}
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				elementDescription.Format = DXGI_FORMAT_R32_SINT;
			}
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				elementDescription.Format = DXGI_FORMAT_R32_FLOAT;
			}
			byteOffset += 4;
		}
		else if(parameterDescription.Mask <= 3)
		{
			if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32) 
			{
				elementDescription.Format = DXGI_FORMAT_R32G32_UINT;
			}
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				elementDescription.Format = DXGI_FORMAT_R32G32_SINT;
			}
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				elementDescription.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			byteOffset += 8;
		}
		else if(parameterDescription.Mask <= 7)
		{
			if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32) 
			{
				elementDescription.Format = DXGI_FORMAT_R32G32B32_UINT;
			}
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				elementDescription.Format = DXGI_FORMAT_R32G32B32_SINT;
			}
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				elementDescription.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			byteOffset += 12;
		}
		else if(parameterDescription.Mask <= 15)
		{
			if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				elementDescription.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			}
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				elementDescription.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			}
			else if(parameterDescription.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				elementDescription.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			byteOffset += 16;
		}

		inputLayoutDescription.push_back(elementDescription);
	}

	if(FAILED(m_Device->CreateInputLayout(&inputLayoutDescription[0], inputLayoutDescription.size(),
		p_ShaderData->GetBufferPointer(), p_ShaderData->GetBufferSize(), &m_VertexLayout)))
	{
		throw ShaderException("Input layout creation failed", __LINE__, __FILE__);
	}

	SAFE_RELEASE(shaderReflection);
}

HRESULT Shader::createShader(ID3DBlob *p_ShaderData) 
{
	HRESULT result;
	switch (m_ShaderType)
	{
	case Type::VERTEX_SHADER:
		{
			result = m_Device->CreateVertexShader(p_ShaderData->GetBufferPointer(), p_ShaderData->GetBufferSize(),
				nullptr, &m_VertexShader);
			if(m_VertexLayout == nullptr)
			{
				result = m_Device->CreateInputLayout(m_VertexDescription, m_NumOfElements, p_ShaderData->GetBufferPointer(),
					p_ShaderData->GetBufferSize(), &m_VertexLayout);
			}
			break;
		}
	case Type::PIXEL_SHADER:
		{
			result = m_Device->CreatePixelShader(p_ShaderData->GetBufferPointer(), p_ShaderData->GetBufferSize(),
				nullptr, &m_PixelShader);
			break;
		}
	case Type::GEOMETRY_SHADER:
		{
			result = m_Device->CreateGeometryShader(p_ShaderData->GetBufferPointer(), p_ShaderData->GetBufferSize(),
				nullptr, &m_GeometryShader);
			break;
		}
	case Type::HULL_SHADER:
		{
			result = m_Device->CreateHullShader(p_ShaderData->GetBufferPointer(), p_ShaderData->GetBufferSize(),
				nullptr, &m_HullShader);
			break;
		}
	case Type::DOMAIN_SHADER:
		{
			result = m_Device->CreateDomainShader(p_ShaderData->GetBufferPointer(), p_ShaderData->GetBufferSize(),
				nullptr, &m_DomainShader);
			break;
		}
	default:
		{
			result = S_FALSE;
			return result;
			break;
		}
	}
	return result;
}

HRESULT Shader::compileShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
	DWORD p_ShaderFlags, ID3DBlob *&p_ShaderData, ID3DBlob *&p_ErrorMessage )
{
	return D3DCompileFromFile(p_Filename, nullptr, nullptr, p_EntryPoint, p_ShaderModel,
		p_ShaderFlags, 0, &p_ShaderData, &p_ErrorMessage);
}
