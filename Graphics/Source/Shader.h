#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
//#include <d3d11shader.h>

#include "Util.h"
#include "MyGraphicsExceptions.h"

#pragma comment(lib, "d3dcompiler.lib")

class Shader
{
public:
	enum class ShaderType
	{
		VERTEX_SHADER,
		PIXEL_SHADER,
		GEOMETRY_SHADER,
		HULL_SHADER,
		DOMAIN_SHADER
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
	
	ShaderType m_ShaderType;
	UINT m_NumOfElements;
public:
	Shader(void);
	//Shader(Shader &p_Other);
	//Shader(Shader &&p_Other);
	~Shader(void);

	//Shader &operator=(const Shader &rhs);
	//Shader &Shader::operator=(Shader &&rhs);

	void initialize(ID3D11Device *p_Device,	ID3D11DeviceContext *p_DeviceContext, unsigned int p_NumOfElements);
	HRESULT compileAndCreateShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
		ShaderType p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout);

	void setShader(void);
	void unSetShader(void);
	
	void setResource(ShaderType p_ShaderType, UINT p_StartSpot,
		UINT p_NumOfViews, ID3D11ShaderResourceView *p_ShaderResource);
	void setSamplerState(ShaderType p_ShaderType, UINT p_StartSpot,
		UINT p_NumOfSamples, ID3D11SamplerState *p_SamplerState);
	void setBlendState(ID3D11BlendState *p_BlendState);

private:
	

protected:
	virtual HRESULT createShader(ID3DBlob *p_ShaderData);
	virtual void createInputLayoutFromShaderSignature(ID3DBlob *p_ShaderData);
	virtual HRESULT compileShader(LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, DWORD p_ShaderFlags,
		ID3DBlob *&p_ShaderData, ID3DBlob *&p_ErrorMessage);
};