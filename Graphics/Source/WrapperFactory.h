#pragma once
#include "Shader.h"
#include "Buffer.h"
class WrapperFactory
{
private:
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;

public:
	WrapperFactory(ID3D11Device *p_Device,
		ID3D11DeviceContext *p_DeviceContext);
	~WrapperFactory(void);

	Shader *createShader(LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, ShaderType p_ShaderType);
	Shader *createShader(LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, ShaderType p_ShaderType,
		const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout,
		unsigned int p_NumOfInputElemts);
	
	Buffer *createBuffer(BufferDescription &p_Description);
};