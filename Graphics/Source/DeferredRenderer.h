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

class DeferredRenderer
{
private:
	std::vector<Renderable> m_TransparentObjects;
	std::vector<Renderable> m_Objects;
	std::vector<PointLight> m_PointLights;

	ID3D11RenderTargetView		*m_RenderTargets[3];
	ID3D11ShaderResourceView	*m_DepthStencilResourceView;
	ID3D11ShaderResourceView	*m_DiffuseSRV;
	ID3D11ShaderResourceView	*m_NormalSRV; // Normal.xy and specular data.
	ID3D11ShaderResourceView	*m_LightSRV;
	//ID3D11RenderTargetView	*m_LightRenderTarget;

	ID3D11Device				*m_Device;
	ID3D11DeviceContext			*m_DeviceContext;
	ID3D11DepthStencilView		*m_DepthStencilView;

	Shader						*m_LightShader;	
	Buffer						*m_LightBuffer;

public:
	DeferredRenderer(void);
	~DeferredRenderer(void);

	void initialize(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext,
					ID3D11DepthStencilView *p_DepthStencilView,
					unsigned int p_screenWidth, unsigned int p_screenHeight);
	void shutdown();

	void renderDeferred();
	
	void addRenderable(Renderable p_Renderable, bool p_Transparent);
	void addLight();

private:
	void renderGeometry();
	void renderLighting();
	void renderFinal();


	void renderForward();
};

