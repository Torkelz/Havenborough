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

		string temp = getShaderModel(p_ShaderModel, p_ShaderType);
		p_Shader->compileAndCreateShader(p_Filename, p_EntryPoint, temp.c_str(), p_ShaderType, nullptr);

}

void WrapperFactory::addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, Shader::Type p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout)
{

		string temp = getShaderModel(p_ShaderModel, p_ShaderType);
		p_Shader->compileAndCreateShader(p_Filename, p_EntryPoint, temp.c_str(), p_ShaderType, p_VertexLayout);
}

Buffer *WrapperFactory::createBuffer(Buffer::Description &p_Description)
{
	Buffer *buffer = new Buffer();
	HRESULT result;
	UINT32 bufferSize = 0;

	if(p_Description.usage != Buffer::Usage::STAGING)
		bufferSize = p_Description.sizeOfElement * p_Description.numOfElements;

	try
	{
		result = buffer->initialize(m_Device, m_DeviceContext, p_Description);
		if(result == S_OK)
		{
			VRAMMemInfo::getInstance()->updateUsage(bufferSize);
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

string WrapperFactory::getShaderModel(const char *p_ShaderVersion, Shader::Type p_Type)
{
	string temp;

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


Shader *WrapperFactory::createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
	ShaderType p_Type)
{
	Shader *shader = new Shader();
	shader->initialize(m_Device, m_DeviceContext, 0);

	vector<string> entryPointList = createEntryPointList(p_EntryPoint);
	string entryPoint;

	try
	{
		for(int i = 0; i < 5; i++)
		{
			if(p_Type & ShaderType((int)std::pow(2,i)))
			{
				entryPoint = entryPointList.back();
				entryPointList.pop_back();
				addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
					Shader::Type(i));
			}
		}
		//if((p_Type & ShaderType::VERTEX_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::VERTEX_SHADER);
		//}
		//if((p_Type & ShaderType::PIXEL_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::PIXEL_SHADER);
		//}
		//if((p_Type & ShaderType::GEOMETRY_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::GEOMETRY_SHADER);
		//}
		//if((p_Type & ShaderType::HULL_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::HULL_SHADER);
		//}
		//if((p_Type & ShaderType::DOMAIN_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::DOMAIN_SHADER);
		//}

		return shader;
	}
	catch(...)
	{
		SAFE_DELETE(shader);
		throw;
	}
}

Shader *WrapperFactory::createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
	ShaderType p_Type, ShaderInputElementDescription *p_VertexLayout, unsigned int p_NumOfInputElements)
{
	Shader *shader = new Shader();
	shader->initialize(m_Device, m_DeviceContext, p_NumOfInputElements);

	vector<string> entryPointList = createEntryPointList(p_EntryPoint);
	string entryPoint;

	D3D11_INPUT_ELEMENT_DESC *desc = new D3D11_INPUT_ELEMENT_DESC[p_NumOfInputElements];
	for(unsigned int i = 0; i < p_NumOfInputElements; i++)
	{
		desc[i].SemanticName = p_VertexLayout[i].semanticName;
		desc[i].SemanticIndex = p_VertexLayout[i].semanticIndex; 
		desc[i].Format = (DXGI_FORMAT)p_VertexLayout[i].format;
		desc[i].InputSlot = p_VertexLayout[i].inputSlot;
		desc[i].AlignedByteOffset = p_VertexLayout[i].alignedByteOffset;
		desc[i].InputSlotClass = (D3D11_INPUT_CLASSIFICATION)p_VertexLayout[i].inputSlotClass;
		desc[i].InstanceDataStepRate = p_VertexLayout[i].instanceDataStepRate;
	}

	try
	{
		for(int i = 0; i < 5; i++)
		{
			if(p_Type & ShaderType((int)std::pow(2,i)))
			{
				entryPoint = entryPointList.back();
				entryPointList.pop_back();
				addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
					Shader::Type(i), desc);
			}
		}
		//if((p_Type & ShaderType::VERTEX_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::VERTEX_SHADER, desc);
		//}
		//if((p_Type & ShaderType::PIXEL_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::PIXEL_SHADER, desc);
		//}
		//if((p_Type & ShaderType::GEOMETRY_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::GEOMETRY_SHADER, desc);
		//}
		//if((p_Type & ShaderType::HULL_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::HULL_SHADER, desc);
		//}
		//if((p_Type & ShaderType::DOMAIN_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::DOMAIN_SHADER, desc);
		//}

		return shader;
	}
	catch(...)
	{
		SAFE_DELETE(shader);
		throw;
	}
}

void WrapperFactory::addShader(Shader *p_Shader, LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, ShaderType p_Type)
{
	vector<string> entryPointList = createEntryPointList(p_EntryPoint);
	string entryPoint;
	
	if(p_Type & ShaderType::VERTEX_SHADER)
	{
		throw WrapperFactoryException("Failed to create shader, vertex shader already exists", __LINE__, __FILE__);
	}

	try
	{
		for(int i = 0; i < 5; i++)
		{
			if(p_Type & ShaderType((int)std::pow(2,i)))
			{
				entryPoint = entryPointList.back();
				entryPointList.pop_back();
				addShaderStep(p_Shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
					Shader::Type(i));
			}
		}
		//if((p_Type & ShaderType::VERTEX_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::VERTEX_SHADER);
		//}
		//if((p_Type & ShaderType::PIXEL_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::PIXEL_SHADER);
		//}
		//if((p_Type & ShaderType::GEOMETRY_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::GEOMETRY_SHADER);
		//}
		//if((p_Type & ShaderType::HULL_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::HULL_SHADER);
		//}
		//if((p_Type & ShaderType::DOMAIN_SHADER))
		//{
		//	entryPoint = entryPointList.back();
		//	entryPointList.pop_back();
		//	addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
		//		Shader::Type::DOMAIN_SHADER);
		//}
	}
	catch(...)
	{
		throw;
	}
}

vector<string> WrapperFactory::createEntryPointList(const char *p_EntryPoint)
{
	vector<string> entryList;
	vector<string> result;

	std::vector<char> buffer(strlen(p_EntryPoint)+1);
	strcpy(buffer.data(), p_EntryPoint);
	char *tmp;
	tmp = strtok(buffer.data(), ",");
	while(tmp != nullptr)
	{
		entryList.push_back(tmp);
		tmp = strtok(NULL,",");
	}

	for(int i = entryList.size() - 1; i >= 0; i--)
	{
		result.push_back(entryList.at(i));
	}

	return result;
}