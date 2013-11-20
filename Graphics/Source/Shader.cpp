#include "Shader.h"
#include <string>
#include <iostream>

Shader::Shader(void)
{
}


Shader::~Shader(void)
{
}

bool Shader::initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, unsigned int p_NumOfElements)
{
	m_Device = p_Device;
	m_DeviceContext = p_DeviceContext;
	m_NumOfElements = p_NumOfElements;

	return true;
}

HRESULT Shader::compileAndCreateShader(LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, ShaderType p_ShaderType,
	const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout)
{
	HRESULT result = S_FALSE;
	try
	{
		
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
		ID3DBlob *errorMessage;
		ID3DBlob *shaderData;


		result = D3DCompileFromFile(p_Filename, nullptr, nullptr, p_EntryPoint, p_ShaderModel,
			shaderFlags, 0, &shaderData, &errorMessage);
		if(FAILED(result))
		{
			throw ShaderException("Error when compiling shader.\n" + (std::string)(char*)errorMessage->GetBufferPointer(),
				__LINE__, __FILE__);

			errorMessage->Release();
			errorMessage = nullptr;

			return result;
		}

		switch (p_ShaderType)
		{
		case VERTEX_SHADER:
			{
				result = m_Device->CreateVertexShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(),
					nullptr, &m_VertexShader);
				result = m_Device->CreateInputLayout(p_VertexLayout, m_NumOfElements, shaderData->GetBufferPointer(),
					shaderData->GetBufferSize(), &m_VertexLayout);
				break;
			}
		case PIXEL_SHADER:
			{
				result = m_Device->CreatePixelShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(),
					nullptr, &m_PixelShader);
				break;
			}
		case GEOMETRY_SHADER:
			{
				result = m_Device->CreateGeometryShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(),
					nullptr, &m_GeometryShader);
				break;
			}
		case HULL_SHADER:
			{
				result = m_Device->CreateHullShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(),
					nullptr, &m_HullShader);
				break;
			}
		case DOMAIN_SHADER:
			{
				result = m_Device->CreateDomainShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(),
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

		if(FAILED(result))
		{
			throw ShaderException("Error when creating.\n" + (std::string)(char*)errorMessage->GetBufferPointer(),
				__LINE__, __FILE__);
		}

		shaderData->Release();
		errorMessage->Release();

		return result;
	}
	catch (MyException *e)
	{
		std::cout << e->what() << std::endl;
		return result;
	}	
}

void Shader::setShader(void)
{
	if(m_VertexShader)
	{
		m_DeviceContext->IASetInputLayout(m_VertexLayout);
		m_DeviceContext->VSSetShader(m_VertexShader,0,0);
	}
	else
	{
		m_DeviceContext->VSSetShader(nullptr, nullptr, 0);
	}

	if(m_PixelShader)
	{
		m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);
	}
	else
	{
		m_DeviceContext->PSSetShader(nullptr, nullptr, 0);
	}

	if(m_GeometryShader)
	{
		m_DeviceContext->GSSetShader(m_GeometryShader, nullptr, 0);
	}
	else
	{
		m_DeviceContext->GSSetShader(nullptr, nullptr, 0);
	}

	if(m_DomainShader)
	{
		m_DeviceContext->DSSetShader(m_DomainShader, nullptr, 0);
	}
	else
	{
		m_DeviceContext->DSSetShader(nullptr, nullptr, 0);
	}

	if(m_HullShader)
	{
		m_DeviceContext->HSSetShader(m_HullShader, nullptr, 0);
	}
	else
	{
		m_DeviceContext->HSSetShader(nullptr, nullptr, 0);
	}
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

void Shader::setResource(ShaderType p_ShaderType, UINT p_StartSpot, UINT p_NumOfViews,
	ID3D11ShaderResourceView *p_ShaderResource)
{
	switch(p_ShaderType)
	{
	case VERTEX_SHADER:
		{
			m_DeviceContext->VSSetShaderResources(p_StartSpot, p_NumOfViews, &p_ShaderResource);
			break;
		}
	case PIXEL_SHADER:
		{
			m_DeviceContext->PSSetShaderResources(p_StartSpot, p_NumOfViews, &p_ShaderResource);
			break;
		}
	case GEOMETRY_SHADER:
		{
			m_DeviceContext->GSSetShaderResources(p_StartSpot, p_NumOfViews, &p_ShaderResource);
			break;
		}
	case HULL_SHADER:
		{
			m_DeviceContext->HSSetShaderResources(p_StartSpot, p_NumOfViews, &p_ShaderResource);
			break;
		}
	case DOMAIN_SHADER:
		{
			m_DeviceContext->DSSetShaderResources(p_StartSpot, p_NumOfViews, &p_ShaderResource);
			break;
		}
	}
}

void Shader::setSamplerState(ShaderType p_ShaderType, UINT p_StartSpot, UINT p_NumOfSamples,
	ID3D11SamplerState *p_SamplerState)
{
	switch (p_ShaderType)
	{
	case VERTEX_SHADER:
		{
			m_DeviceContext->VSSetSamplers(p_StartSpot, p_NumOfSamples, &p_SamplerState);
			break;
		}
	case PIXEL_SHADER:
		{
			m_DeviceContext->PSSetSamplers(p_StartSpot, p_NumOfSamples, &p_SamplerState);
			break;
		}
	case GEOMETRY_SHADER:
		{
			m_DeviceContext->GSSetSamplers(p_StartSpot, p_NumOfSamples, &p_SamplerState);
			break;
		}
	case HULL_SHADER:
		{
			m_DeviceContext->HSSetSamplers(p_StartSpot, p_NumOfSamples, &p_SamplerState);
			break;
		}
	case DOMAIN_SHADER:
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

void Shader::setBlendState(ID3D11BlendState *p_BlendState)
{
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	UINT sampleMask = 0xffffffff;
	m_DeviceContext->OMSetBlendState(p_BlendState, blendFactor, sampleMask);
}