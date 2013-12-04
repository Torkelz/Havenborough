#pragma once
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <memory>
#include <vector>
#include "Shader.h"
#include "Buffer.h"
#include "Util.h"
#include <DirectXMath.h>
#include "LightStructs.h"
#include "TextureLoader.h"

struct Renderable
{
	//std::shared_ptr<Model>	m_Model;
	Buffer				*m_Buffer; //Should be replaced by a Model object at a later date
	Buffer				*m_ConstantBuffer;
	Shader				*m_Shader;
	DirectX::XMFLOAT4X4 *m_World;

	Renderable(Buffer* p_Buffer, Buffer* p_ConstantBuffer,
		Shader* p_Shader, DirectX::XMFLOAT4X4* p_World)
	{
		m_Buffer = p_Buffer;
		m_ConstantBuffer = p_ConstantBuffer;
		m_Shader = p_Shader;
		m_World = p_World;
	}

	~Renderable()
	{
		m_Buffer = nullptr;
		m_ConstantBuffer = nullptr;
		m_Shader = nullptr;
		m_World = nullptr;
	}
};

struct cBuffer
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT3	campos;
	int					nrLights;
};


class DeferredRenderer
{
private:
	std::vector<Renderable>		m_Objects;
	std::vector<Light>			m_Lights;

	ID3D11Device				*m_Device;
	ID3D11DeviceContext			*m_DeviceContext;
	ID3D11DepthStencilView		*m_DepthStencilView;

	static const unsigned int	m_numRenderTargets = 4;
	ID3D11RenderTargetView		*m_RenderTargets[m_numRenderTargets];

	ID3D11ShaderResourceView	*m_DiffuseSRV;
	ID3D11ShaderResourceView	*m_NormalSRV; // Normal.xy and specular data.
	ID3D11ShaderResourceView	*m_LightSRV;
	ID3D11ShaderResourceView	*m_wPositionSRV;
	ID3D11ShaderResourceView	*m_lightBufferSRV;

	ID3D11SamplerState			*m_Sampler;
	ID3D11BlendState			*m_BlendState;
	ID3D11BlendState			*m_BlendState2;
	Shader						*m_LightShader;
	Buffer						*m_ConstantBuffer;
	Buffer						*m_AllLightBuffer;

	DirectX::XMFLOAT3			*m_CameraPosition;
	DirectX::XMFLOAT4X4			*m_ViewMatrix;
	DirectX::XMFLOAT4X4			*m_ProjectionMatrix;


	//TEMP--------------------------------------------------
	float						m_speed;
	int							xx,yy,zz;

	TextureLoader				*m_TextureLoader; // TEST
	ID3D11ShaderResourceView	*m_Specular, *m_Diffuse, *m_NormalMap;
	//TEMP---------------------------------------------------

public:
	DeferredRenderer(void);
	~DeferredRenderer(void);

	void initialize(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext,
		ID3D11DepthStencilView *p_DepthStencilView,
		unsigned int p_screenWidth, unsigned int p_screenHeight,
		DirectX::XMFLOAT3 *p_CameraPosition, DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix);



	void shutdown();

	void renderDeferred();

	void addRenderable(Renderable p_Renderable);
	void addLight(Light p_light);

	ID3D11ShaderResourceView* getRT(int i); //DEBUG
private:
	void renderGeometry();

	void clearRenderTargets( unsigned int nrRT );

	void renderLighting();

	void updateConstantBuffer(int nrLights);
	void updateLightBuffer();

	HRESULT createRenderTargets(D3D11_TEXTURE2D_DESC &desc);
	HRESULT createShaderResourceViews( ID3D11DepthStencilView * p_DepthStencilView, D3D11_TEXTURE2D_DESC &desc );
	void createConstantBuffer(int nrLights);
	void clearRenderTargets();
	void createSamplerState();
	void createBlendStates();
};

