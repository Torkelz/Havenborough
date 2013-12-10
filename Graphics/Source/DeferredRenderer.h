#pragma once
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <memory>
#include <vector>
//#include "Shader.h"
//#include "Buffer.h"
#include "WrapperFactory.h"
#include "Util.h"
#include <DirectXMath.h>
#include "LightStructs.h"
#include "TextureLoader.h"
#include "ModelDefinition.h"



/*
 * cBuffer contains the matrices needed to render the models and lights.
 */
struct cBuffer
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT3	campos;
	int					nrLights;
};
struct cObjectBuffer
{
	DirectX::XMFLOAT4X4 world;
};

class DeferredRenderer
{
public:
	/*
	 * Renderable is a debug struct made with the only purpose to be a placeholder for models
	 * until the model loader is done.
	 */
	struct Renderable
	{
		Model				*m_Model;
		const DirectX::XMFLOAT4X4 *m_World;

		Renderable(Model *p_Model, const DirectX::XMFLOAT4X4* p_World)
		{
			m_Model = p_Model;
			m_World = p_World;
		}

		~Renderable()
		{
			m_Model = nullptr;
			m_World = nullptr;
		}
	};
private:
	std::vector<Renderable>		m_Objects;
	std::vector<Light>			m_Lights;

	ID3D11Device				*m_Device;
	ID3D11DeviceContext			*m_DeviceContext;
	ID3D11DepthStencilView		*m_DepthStencilView;

	static const unsigned int	m_numRenderTargets = 4;
	ID3D11RenderTargetView		*m_RenderTargets[m_numRenderTargets];

	ID3D11ShaderResourceView	*m_DiffuseSRV;
	ID3D11ShaderResourceView	*m_NormalSRV;
	ID3D11ShaderResourceView	*m_LightSRV;
	ID3D11ShaderResourceView	*m_wPositionSRV;
	ID3D11ShaderResourceView	*m_lightBufferSRV;

	ID3D11SamplerState			*m_Sampler;
	ID3D11BlendState			*m_BlendState;
	ID3D11BlendState			*m_BlendState2;
	Shader						*m_LightShader;
	Buffer						*m_ConstantBuffer;
	Buffer						*m_ObjectConstantBuffer;
	Buffer						*m_AllLightBuffer;

	DirectX::XMFLOAT3			m_CameraPosition;
	DirectX::XMFLOAT4X4			m_ViewMatrix;
	DirectX::XMFLOAT4X4			m_ProjectionMatrix;


	//TEMP--------------------------------------------------
	float						m_speed;
	int							xx,yy,zz;

	//TEMP---------------------------------------------------

public:
	DeferredRenderer(void);
	~DeferredRenderer(void);

	/*
	 * Initialize all the needed variables for rendering.
	 * @ p_Device, DirectX Device used for rendering
	 * @ p_DeviceContect, DX device context. Used for rendering.
	 * @ p_DepthStencilView, used for z-culling when rendering.
	 * @ p_ScreenWidth, used to initialize render textures.
	 * @ p_ScreenHeight, used to initialize render textures.
	 * @ p_CameraPosition, the camera position. Used when rendering.
	 * @ p_ViewMatrix, the view matrix. Used when rendering.
	 * @ p_ProjectionMatrix, the projection matrix. Used when rendering.
	 */
	void initialize(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext,
		ID3D11DepthStencilView *p_DepthStencilView,
		unsigned int p_ScreenWidth, unsigned int p_ScreenHeight,
		const DirectX::XMFLOAT3& p_CameraPosition, const DirectX::XMFLOAT4X4& p_ViewMatrix,
		const DirectX::XMFLOAT4X4& p_ProjectionMatrix);

	/*
	 * Call to render the graphics using deferred rendering.
	 * All the objects that are supposed to be rendered must have been sent to the renderer
	 * before calling this function.
	 */
	void renderDeferred();

	/*
	 * Add models to the list of objects to be rendered with deferred rendering.
	 * @ p_Renderable, the model that needs to be rendered.
	 */
	void addRenderable(Renderable p_Renderable);
	/*
	 * Add light source to the list of lights to be taken into the light calculations.
	 * @ p_Light, light source that needs to be taken into the light calculations.
	 */
	void addLight(Light p_light);

	/*
	 * Use to get specific render targets to put on the back buffer.
	 * @ i, a number that is associated with a render target view.
	 * @return, render target if i is a legal number, else nullptr.
	 */
	ID3D11ShaderResourceView* getRT(int i); //DEBUG

	/**
	 * Update the view matrix. Remember to also update the
	 * camera position if it has changed.
	 *
	 * @param p_ViewMat the new view matrix.
	 */
	void updateViewMatrix(const DirectX::XMFLOAT4X4& p_ViewMat);

	/**
	 * Update the camera position. Remember to also update the
	 * view matrix if it has changed.
	 *
	 * @param p_CameraPos the new camera position in absolute world coordinates.
	 */
	void updateCameraPosition(const DirectX::XMFLOAT3& p_CameraPos);
	
private:
	void renderGeometry();

	void clearRenderTargets( unsigned int nrRT );

	void renderLighting();

	void updateConstantBuffer(int nrLights);
	void updateLightBuffer();

	HRESULT createRenderTargets(D3D11_TEXTURE2D_DESC &desc);
	HRESULT createShaderResourceViews( D3D11_TEXTURE2D_DESC &desc );
	void createConstantBuffer(int nrLights);
	void clearRenderTargets();
	void createSamplerState();
	void createBlendStates();
};

