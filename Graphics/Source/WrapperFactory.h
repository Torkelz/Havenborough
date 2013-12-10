#pragma once
#include <string>
#include <vector>
#include "Shader.h"
#include "Buffer.h"
#include "VRAMMemInfo.h"
#include "../include/ShaderDeffinitions.h"

using std::string;
using std::vector;

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
	* Creates a buffer from a buffer description.
	* @param p_Description the description the buffer should use
	* @return pointer to the created buffer
	*/
	virtual Buffer *createBuffer(Buffer::Description &p_Description);




	virtual Shader *createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel, ShaderType p_Type);

	virtual Shader *createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel, ShaderType p_Type,
		ShaderInputElementDescription *p_VertexLayout, unsigned int p_NumOfInputElements);

	virtual void addShader(Shader *p_Shader, LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
		ShaderType p_Type);

protected:
	virtual vector<string> createEntryPointList(const char *p_EntryPoint);

	/**
	* Automatically creates a shader based on layout in the shader file.
	* @param p_Shader the shader object where to store the new shader
	* @param p_Filename the shader file to read
	* @param p_EntryPoint the main entry point in the shader file
	* @param p_ShaderModel the shader model version to be used
	* @param p_ShaderType the type of shader to create
	*/
	virtual void addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, Shader::Type p_ShaderType);

	/**
	* Automatically creates a shader based on user defined layout. Should only be used if defining a vertex shader.
	* @param p_Shader the shader object where to store the new shader
	* @param p_Filename the shader file to read
	* @param p_EntryPoint the main entry point in the shader file
	* @param p_ShaderModel the shader model version to be used
	* @param p_ShaderType the type of shader to create
	* @param p_VertexLayout the user defined vertex layout shader should use
	*/
	virtual void addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, Shader::Type p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout);

protected:
	WrapperFactory(void);
	virtual ~WrapperFactory(void);
	
	virtual string getShaderModel(const char *p_ShaderVersion, Shader::Type p_Type);
};