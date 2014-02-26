#pragma once
#include "IGraphics.h"
#include "Light.h"
#include "Renderable.h"
#include "SkyDome.h"
#include "ConstantBuffers.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <map>

class DeferredRenderer
{
private:
	float m_FOV;
	float m_FarZ;
	float m_ScreenWidth;
	float m_ScreenHeight;

	std::vector<Renderable>	m_Objects;

	ID3D11Device			*m_Device;
	ID3D11DeviceContext		*m_DeviceContext;
	ID3D11DepthStencilView	*m_DepthStencilView;

	std::vector<Light>		*m_SpotLights;
	std::vector<Light>		*m_PointLights;
	std::vector<Light>		*m_DirectionalLights;
	Light					*m_ShadowMappedLight;
	unsigned int			m_MaxLightsPerLightInstance;

	DirectX::XMFLOAT3		m_CameraPosition;
	DirectX::XMFLOAT4X4		*m_ViewMatrix;
	DirectX::XMFLOAT4X4		*m_ProjectionMatrix;

	static const unsigned int	m_numRenderTargets = 5;

	std::map<IGraphics::RenderTarget, ID3D11RenderTargetView*> m_RT;
	std::map<std::string, ID3D11ShaderResourceView*> m_SRV;
	std::map<std::string, ID3D11SamplerState*> m_Sampler;
	std::map<std::string, Shader*> m_Shader;
	std::map<std::string, Buffer*> m_Buffer;

	ID3D11BlendState	*m_BlendState;
	ID3D11BlendState	*m_BlendState2;

	ID3D11RasterizerState	*m_RasterState;
	ID3D11DepthStencilState	*m_DepthState;

	bool	m_RenderSkyDome;
	SkyDome	*m_SkyDome;

	bool	m_SSAO;
	bool	m_ShadowMap;
	int		m_ShadowMapResolution;

	ID3D11ShaderResourceView*	m_DepthMapSRV;
	ID3D11DepthStencilView*		m_DepthMapDSV;
	UINT						m_Width;
	UINT						m_Height;

	float						m_ViewHW;
	DirectX::XMFLOAT4X4			m_LightView;
	DirectX::XMFLOAT4X4			m_LightProjection;
	D3D11_VIEWPORT				m_LightViewport;
	unsigned int				m_MaxNumDirectionalShadows;


public:
	/**
	* Constructor. 
	*/
	DeferredRenderer(void);


	/**
	* Destructor.
	*/
	~DeferredRenderer(void);


	/*
	 * Initialize all the needed variables for rendering.
	 * 
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
		ID3D11DepthStencilView *p_DepthStencilView, unsigned int p_ScreenWidth, unsigned int p_ScreenHeight,
		DirectX::XMFLOAT3 p_CameraPosition, DirectX::XMFLOAT4X4 *p_ViewMatrix,
		DirectX::XMFLOAT4X4 *p_ProjectionMatrix,int p_ShadowMapResolution, std::vector<Light> *p_SpotLights,
		std::vector<Light> *p_PointLights, std::vector<Light> *p_DirectionalLights, Light *p_ShadowMappedLight,
		unsigned int p_MaxLightsPerLightInstance, float p_FOV, float p_FarZ);

	/*
	* Creates the shadow map Texture2D desc, depthMap, depthStencilViewDesc, ShaderResourceViewDesc.
	*
	*/
	void initializeShadowMap(UINT width, UINT height);



	/*
	 * Call to render the graphics using deferred rendering.
	 *
	 * All the objects that are supposed to be rendered must have been sent to the renderer
	 * before calling this function.
	 */
	void renderDeferred();


	/*
	 * Add models to the list of objects to be rendered with deferred rendering.
	 *
	 * @ p_Renderable, the model that needs to be rendered.
	 */
	void addRenderable(Renderable p_Renderable);
	/*
	 * Add models to the list of objects to be rendered with deferred rendering.
	 * @ p_Texture, the texture for the skydome
	 * @ p_Radius, the radius of the skydome.
	 */
	void createSkyDome(ID3D11ShaderResourceView* p_Texture, float p_Radius);
	/*
	 * Tells the deferred renderer to render the skyDome created.
	 */
	void renderSkyDome();


	/*
	 * Use to get specific render targets to put on the back buffer.
	 * @ i, a number that is associated with a render target view.
	 * @return, render target if i is a legal number, else nullptr.
	 */
	ID3D11ShaderResourceView* getRT(IGraphics::RenderTarget i); //DEBUG

	void updateCamera(DirectX::XMFLOAT3 p_Position);

	void enableSSAO(bool p_State);

	void enableShadowMap(bool p_State);

private:
	void renderGeometry(ID3D11DepthStencilView*, unsigned int, ID3D11RenderTargetView* rtv[]);
	void renderSSAO(void);
	void blurSSAO(void);
	void SSAO_PingPong(ID3D11ShaderResourceView*, ID3D11RenderTargetView*, bool p_HorizontalBlur);
	void updateSSAO_BlurConstantBuffer(bool p_HorizontalBlur);
	void updateSSAO_VarConstantBuffer();


	void clearRenderTargets(unsigned int nrRT);


	void renderLighting();
	void renderSkyDomeImpl();


	void renderLight(Shader *p_Shader, Buffer *p_ModelBuffer, std::vector<Light> *p_Lights);


	void updateConstantBuffer(DirectX::XMFLOAT4X4 p_ViewMatrix, DirectX::XMFLOAT4X4 p_ProjMatrix);
	void updateLightBuffer(bool p_Big, bool p_ShadowMapped);


	HRESULT createRenderTargets(D3D11_TEXTURE2D_DESC &desc);
	HRESULT createShaderResourceViews(D3D11_TEXTURE2D_DESC &desc);
	void createBuffers();
	void buildSSAO_OffsetVectors(cSSAO_Buffer &p_Buffer);
	void clearRenderTargets();
	void createSamplerState();
	void createBlendStates();
	void createShaders();
	void loadLightModels();
	void createLightStates(); //Rasterize and depth state
	void createRandomTexture(unsigned int p_Size);


	void renderObject(Renderable &p_Object);
	void SortRenderables( std::vector<Renderable> &animatedOrSingle, std::vector<std::vector<Renderable>> &instancedModels );
	void RenderObjectsInstanced( std::vector<Renderable> &p_Objects );

	void updateLightView(DirectX::XMFLOAT3 p_Dir);
	void updateLightProjection(float p_viewHW);
	void renderDirectionalLights(Light p_Directional);
};