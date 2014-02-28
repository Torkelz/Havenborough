#pragma once
#pragma comment(lib, "d3dcompiler.lib")
#define NUM_OF_SHADER_TYPES 5

#include <d3d11.h>

class Shader
{
public:
	enum class Type
	{
		VERTEX_SHADER = 0,
		PIXEL_SHADER = 1,
		GEOMETRY_SHADER = 2,
		HULL_SHADER = 3,
		DOMAIN_SHADER = 4
	};

private:
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;
	
	ID3D11VertexShader *m_VertexShader;
	ID3D11GeometryShader *m_GeometryShader;
	ID3D11PixelShader *m_PixelShader;	
	ID3D11HullShader *m_HullShader;
	ID3D11DomainShader *m_DomainShader;
	ID3D11InputLayout *m_VertexLayout;
	D3D11_INPUT_ELEMENT_DESC *m_VertexDescription;
	
	Type m_ShaderType;
	UINT m_NumOfElements;

public:
	/**
	* Constructor.
	*/
	Shader(void);

	/**
	* Destructor.
	*/
	~Shader(void);
	
	/**
	* Initializes the shader object with the DirectX API.
	* @param p_Device pointer to the Direc3D device in use
	* @param p_DeviceContext pointer to the Direct3D device context in use
	* @param p_NumOfElements the number of elements in the input layout. Use 0 if the layout is auto generated.
	*/
	void initialize(ID3D11Device *p_Device,	ID3D11DeviceContext *p_DeviceContext, unsigned int p_NumOfElements);
	
	/**
	* Compiles the shader file and initialize the D3D11Shader object based on shader type.
	* @param p_Filename the shader file to read
	* @param p_EntryPoint the main entry point in the shader file
	* @param p_ShaderModel the shader model version to be used
	* @param p_ShaderType the type of shader to create
	* @param p_VertexLayout the user defined vertex layout shader should use
	*/
	HRESULT compileAndCreateShader(LPCWSTR p_Filename, D3D_SHADER_MACRO* p_Defines, const char *p_EntryPoint, const char *p_ShaderModel,
		Type p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout);

	/**
	* Sets this shader object to be active. Use before it should be drawn.
	*/
	void setShader(void);
	
	/**
	* Unsets this shader object from being active. Use after it has been drawn.
	*/
	void unSetShader(void);
	
	/**
	* Connects a resource to a shader.
	* @param p_ShaderType the shader the resource should be connected to
	* @param p_StartSpot which object that should be used as start, usually this should be 0
	* @param p_NumOfViews the number of resources sent to the shader
	* @param P_ShaderResource pointer the the resource(s)
	*/
	void setResource(Type p_ShaderType, UINT p_StartSpot,
		UINT p_NumOfViews, ID3D11ShaderResourceView *p_ShaderResource);
	
	/**
	* Sets a sampler state to a shader.
	* @param p_ShaderType the shader the sampler state should be connected to
	* @param p_StartSpot which object that should be used as start, usually this should be 0
	* @param p_NumOfViews the number of sampler states sent to the shader
	* @param P_SamplerState pointer the the sampler state(s)
	*/
	void setSamplerState(Type p_ShaderType, UINT p_StartSpot,
		UINT p_NumOfSamples, ID3D11SamplerState *p_SamplerState);
	
	/**
	* Sets the blend state for the shader object.
	* @param p_BlendState the blend state that should be used at the moment
	* @param p_BlendFactor the color channels to be blended
	* @param p_SampleMask determines which samples to blend, default is 0xffffffff
	*/
	void setBlendState(ID3D11BlendState *p_BlendState, float p_BlendFactor[4], UINT p_SampleMask = 0xffffffff);
	
private:
	void releaseShader(Type p_Type);

protected:
	virtual HRESULT createShader(ID3DBlob *p_ShaderData);
	virtual void createInputLayoutFromShaderSignature(ID3DBlob *p_ShaderData);
	virtual HRESULT compileShader(LPCWSTR p_Filename, D3D_SHADER_MACRO* p_Defines, const char *p_EntryPoint,	const char *p_ShaderModel, 
		DWORD p_ShaderFlags, ID3DBlob *&p_ShaderData, ID3DBlob *&p_ErrorMessage);
};