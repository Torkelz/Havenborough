#pragma once
#include "Shader.h"
#include "Buffer.h"

class WrapperFactory
{
private:
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;
	static WrapperFactory *m_Instance;

public:
	/**
	* Gets an instance of the wrapper factory. Note that it still needs to be initialized before calling this function.
	* @return a pointer to the instance
	*/
	static WrapperFactory *getInstance(void);

	/**
	* Initialize the factory with device and device context. Should only be called once in Graphics.
	* @param p_Device pointer to the Direc3D device in use
	* @param p_DeviceContext pointer to the Direc3D device context in use
	*/
	static void initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext);

	/**
	* Shuts down the factory and release the memory allocated. Nulls all pointers.
	*/
	virtual void shutdown(void);
	
	/**
	* Automatically creates a shader based on layout in the shader file.
	* @param p_Filename the shader file to read
	* @param p_EntryPoint the main entry point in the shader file
	* @param p_ShaderModel the shader model version to be used
	* @param p_ShaderType the type of shader to create
	*/
	virtual Shader *createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel, Shader::Type p_ShaderType);
	
	/**
	*
	*/
	virtual Shader *createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
		Shader::Type p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout, unsigned int p_NumOfElements);
	
	/**
	*
	*/
	virtual void addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, Shader::Type p_ShaderType);

	/**
	*
	*/
	virtual void addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, Shader::Type p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout);
	
	/**
	*
	*/
	virtual Buffer *createBuffer(Buffer::Description &p_Description);
	
protected:
	WrapperFactory(void);
	virtual ~WrapperFactory(void);
};