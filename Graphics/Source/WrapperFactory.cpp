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
	SAFE_DELETE(m_Instance);
}

void WrapperFactory::addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, Shader::Type p_ShaderType)
{

		std::string temp = getShaderModel(p_ShaderModel, p_ShaderType);
		p_Shader->compileAndCreateShader(p_Filename, p_EntryPoint, temp.c_str(), p_ShaderType, nullptr);

}

void WrapperFactory::addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, Shader::Type p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout)
{

		std::string temp = getShaderModel(p_ShaderModel, p_ShaderType);
		p_Shader->compileAndCreateShader(p_Filename, p_EntryPoint, temp.c_str(), p_ShaderType, p_VertexLayout);
}

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

std::string WrapperFactory::getShaderModel(const char *p_ShaderVersion, Shader::Type p_Type)
{
	std::string temp;

	switch (p_Type)
	{
	case Shader::Type::VERTEX_SHADER:
		{
			temp = "vs_";
			temp += p_ShaderVersion;
			break;
		}
	case Shader::Type::PIXEL_SHADER:
		{
			temp = "ps_";
			temp += p_ShaderVersion;
			break;
		}
	case Shader::Type::GEOMETRY_SHADER:
		{
			temp = "gs_";
			temp += p_ShaderVersion;
			break;
		}
	case Shader::Type::HULL_SHADER:
		{
			temp = "hs_";
			temp += p_ShaderVersion;
			break;
		}
	case Shader::Type::DOMAIN_SHADER:
		{
			temp = "ds_";
			temp += p_ShaderVersion;
			break;
		}
	default:
		{
			return "";
			break;
		}
	}
	return temp;
}

#pragma region OLD STUFF
//Shader *WrapperFactory::createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
//	Shader::Type p_ShaderType)
//{
//	Shader *shader = new Shader();
//
//	try
//	{
//		shader->initialize(m_Device, m_DeviceContext, 0);
//		shader->compileAndCreateShader(p_Filename, p_EntryPoint, p_ShaderModel, p_ShaderType,
//			nullptr);
//
//		return shader;
//	}
//	catch(...)
//	{
//		SAFE_DELETE(shader);
//		throw;
//	}
//}

//Shader *WrapperFactory::createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
//	Shader::Type p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout, unsigned int p_NumOfInputElemts)
//{
//	Shader *shader = new Shader();
//
//	try
//	{
//		shader->initialize(m_Device, m_DeviceContext, p_NumOfInputElemts);
//		shader->compileAndCreateShader(p_Filename, p_EntryPoint, p_ShaderModel, p_ShaderType, p_VertexLayout);
//
//		return shader;
//	}
//	catch(...)
//	{
//		SAFE_DELETE(shader);
//		throw;
//	}
//}
#pragma endregion