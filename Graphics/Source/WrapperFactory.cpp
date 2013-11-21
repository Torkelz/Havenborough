#include "WrapperFactory.h"

WrapperFactory::WrapperFactory(ID3D11Device *p_Device,
							 ID3D11DeviceContext *p_DeviceContext)
							 : m_Device(p_Device),
							  m_DeviceContext(p_DeviceContext)
{
}


WrapperFactory::~WrapperFactory(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
}

Shader *WrapperFactory::createShader(LPCWSTR p_Filename, const char *p_EntryPoint,
									const char *p_ShaderModel, ShaderType p_ShaderType,
									const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout)
{
	Shader* shader = new Shader();
	
	unsigned int numOfInputElements = sizeof(&p_VertexLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	shader->initialize(m_Device, m_DeviceContext, numOfInputElements);
	shader->compileAndCreateShader(p_Filename, p_EntryPoint, p_ShaderModel, p_ShaderType, p_VertexLayout);

	return shader;
}

Buffer *WrapperFactory::createBuffer(BufferDescription &p_Description)
{
	Buffer *buffer = new Buffer();

	buffer->initialize(m_Device, m_DeviceContext, p_Description);

	return buffer;
}