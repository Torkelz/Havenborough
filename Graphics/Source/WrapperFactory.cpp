#include "WrapperFactory.h"

WrapperFactory *WrapperFactory::m_Instance = nullptr;

WrapperFactory *WrapperFactory::getInstance(void)
{
	if(!m_Instance)
		throw WrapperFactoryException("Wrapper factory has not initialized before use", __LINE__, __FILE__);

	return m_Instance;
}

void WrapperFactory::initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext)
{
	if (!m_Instance)
	{
		m_Instance = new WrapperFactory();
	}
	m_Instance->m_Device = p_Device;
	m_Instance->m_DeviceContext = p_DeviceContext;
}

void WrapperFactory::shutdown(void)
{
	//if(!m_Instance)
		//throw WrapperFactoryException("Wrapper factory already shut down or never created.", __LINE__, __FILE__);

	SAFE_DELETE(m_Instance);
}

Shader *WrapperFactory::createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
	Shader::Type p_ShaderType)
{
	Shader *shader = new Shader();

	try
	{
		shader->initialize(m_Device, m_DeviceContext, 0);
		shader->compileAndCreateShader(p_Filename, p_EntryPoint, p_ShaderModel, p_ShaderType,
			nullptr);

		return shader;
	}
	catch(...)
	{
		SAFE_DELETE(shader);
		throw;
	}
}

Shader *WrapperFactory::createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
	Shader::Type p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout, unsigned int p_NumOfInputElemts)
{
	Shader *shader = new Shader();

	try
	{
		shader->initialize(m_Device, m_DeviceContext, p_NumOfInputElemts);
		shader->compileAndCreateShader(p_Filename, p_EntryPoint, p_ShaderModel, p_ShaderType, p_VertexLayout);

		return shader;
	}
	catch(...)
	{
		SAFE_DELETE(shader);
		throw;
	}
}
//void WrapperFactory::addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, const char *p_EntryPoint,
//	const char *p_ShaderModel, int p_ShaderType)
//{
//	/*try
//	{
//	p_Shader->compileAndCreateShader(p_Filename, p_EntryPoint, p_ShaderModel, p_ShaderType, nullptr);
//	}
//	catch(...)
//	{
//	throw;
//	}*/
//}

Buffer *WrapperFactory::createBuffer(Buffer::Description &p_Description)
{
	Buffer *buffer = new Buffer();
	HRESULT result;
	try
	{
		result = buffer->initialize(m_Device, m_DeviceContext, p_Description);
		if(result == S_OK)
		{
			return buffer;
		}
		else
		{
			SAFE_DELETE(buffer);
			return nullptr;
		}
		
	}
	catch(...)
	{
		SAFE_DELETE(buffer);
		throw;
	}
}

WrapperFactory::WrapperFactory(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
}


WrapperFactory::~WrapperFactory(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
}