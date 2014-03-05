#pragma once
#include "Shader.h"
#include "Buffer.h"
#include "ShaderDefinitions.h"

#include <string>
#include <vector>

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
	* Shuts down the factory and releases the memory allocated. Nulls all pointers.
	*/
	virtual void shutdown(void);
	
	/**
	* Creates a buffer from a buffer description.
	* @param p_Description the description the buffer should use
	* @return pointer to the created buffer
	*/
	virtual Buffer *createBuffer(Buffer::Description &p_Description);

	/**
	* Automatically creates a shader based on layout in the shader file.
	* @param p_Filename the shader file to read
	* @param p_EntryPoint the main entry point in the shader file, can be combined as e.g. "mainVS,mainPS,mainGS,mainHS,mainDS",
	*		 note this order is important to be kept but all steps are not necessary,
	*		 note the ',' is the separator
	* @param p_ShaderModel the shader model version to be used, e.g. "5_0"
	* @param p_ShaderType the shader types to be created, can be combined as
	*		 ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER | ShaderType::GEOMETRY_SHADER | ShaderType::HULL_SHADER | ShaderType::DOMAIN_SHADER,
	* @return pointer to the shader object
	*/
	virtual Shader *createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
		ShaderType p_Type);

	/**
	* Automatically creates a shader based on layout in the shader file.
	* @param p_Filename the shader file to read
	* @param p_Defines sets defines in shader files to a value.
	* @param p_EntryPoint the main entry point in the shader file, can be combined as e.g. "mainVS,mainPS,mainGS,mainHS,mainDS",
	*		 note this order is important to be kept but all steps are not necessary,
	*		 note the ',' is the separator
	* @param p_ShaderModel the shader model version to be used, e.g. "5_0"
	* @param p_ShaderType the shader types to be created, can be combined as
	*		 ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER | ShaderType::GEOMETRY_SHADER | ShaderType::HULL_SHADER | ShaderType::DOMAIN_SHADER,
	* @return pointer to the shader object
	*/
	virtual Shader *createShader(LPCWSTR p_Filename, D3D_SHADER_MACRO *p_Defines, const char *p_EntryPoint, const char *p_ShaderModel,
		ShaderType p_Type);

	/**
	* Automatically creates a shader based on user defined layout. Should only be used if defining a vertex shader.
	* @param p_Filename the shader file to read
	* @param p_Defines sets defines in shader files to a value.
	* @param p_EntryPoint the main entry point in the shader file, can be combined as e.g. "mainVS,mainPS,mainGS,mainHS,mainDS",
	*		 note this order is important to be kept but all steps are not necessary,
	*		 note the ',' is the separator
	* @param p_ShaderModel the shader model version to be used, e.g. "5_0"
	* @param p_Type the shader types to be created, can be combined as
	*		 ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER | ShaderType::GEOMETRY_SHADER | ShaderType::HULL_SHADER | ShaderType::DOMAIN_SHADER,
	*		 note that vertex shader needs to be included or an exception will be thrown
	* @param p_VertexLayout the user defined vertex layout shader should use
	* @param p_NumOfElement the number of elements in the layout
	*/
	virtual Shader *createShader(LPCWSTR p_Filename, D3D_SHADER_MACRO *p_Defines, const char *p_EntryPoint, const char *p_ShaderModel,
		ShaderType p_Type, ShaderInputElementDescription *p_VertexLayout, unsigned int p_NumOfInputElements);

	/**
	* Automatically creates a new shader step to a shader based on layout in the shader file.
	* @param p_Shader the shader object where to store the new shader
	* @param p_Filename the shader file to read
	* @param p_Defines sets defines in shader files to a value.
	* @param p_EntryPoint the main entry point in the shader file, can be combined as e.g. "mainVS,mainPS,mainGS,mainHS,mainDS",
	*		 note this order is important to be kept but all steps are not necessary,
	*		 note the ',' is the separator
	* @param p_ShaderModel the shader model version to be used, e.g. "5_0"
	* @param p_ShaderType the shader types to be created, can be combined as
	*		 ShaderType::PIXEL_SHADER | ShaderType::GEOMETRY_SHADER | ShaderType::HULL_SHADER | ShaderType::DOMAIN_SHADER,
	*		 note that an exception will occur if trying to add vertex shader step
	*/
	virtual void addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, D3D_SHADER_MACRO *p_Defines, const char *p_EntryPoint,
		const char *p_ShaderModel, ShaderType p_Type);

protected:
	WrapperFactory(void);
	virtual ~WrapperFactory(void);

	virtual std::string getShaderModel(const char *p_ShaderVersion, Shader::Type p_Type);
	virtual std::vector<std::string> createEntryPointList(const char *p_EntryPoint);

	virtual void addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, D3D_SHADER_MACRO *p_Defines, const char *p_EntryPoint,
		const char *p_ShaderModel, Shader::Type p_ShaderType);
	virtual void addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, D3D_SHADER_MACRO *p_Defines, const char *p_EntryPoint,
		const char *p_ShaderModel, Shader::Type p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout);
};