#pragma once
#include "Shader.h"
#include "Buffer.h"
class WrapperFactory
{
private:
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;

public:
	/**
	* Constructor creating a factory for shader and buffer wrappers.
	* @param p_Device pointer to the Direc3D device in use
	* @param p_DeviceContext pointer to the Direc3D device context in use
	*/
	WrapperFactory(ID3D11Device *p_Device,
		ID3D11DeviceContext *p_DeviceContext);
	~WrapperFactory(void);

	/**
	* Automatically creates a shader based on layout in the shader file.
	* @param p_Filename the shader file to read
	* @param p_EntryPoint the main entry point in the shader file
	* @param p_ShaderModel the shader model version to be used
	* @param p_ShaderType the type of shader to create
	*/
	virtual Shader *createShader(LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, ShaderType p_ShaderType);
	
	/**
	*
	*/
	virtual Shader *createShader(LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, ShaderType p_ShaderType,
		const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout,
		unsigned int p_NumOfInputElemts);
	
	/**
	*
	*/
	virtual Buffer *createBuffer(BufferDescription &p_Description);
};