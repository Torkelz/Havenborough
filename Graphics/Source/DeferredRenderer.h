#pragma once
#include "Light.h"
#include "Renderable.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

class DeferredRenderer
{
private:
	std::vector<Renderable>		m_Objects;

	ID3D11Device				*m_Device;
	ID3D11DeviceContext			*m_DeviceContext;
	ID3D11DepthStencilView		*m_DepthStencilView;

	std::vector<Light>			*m_SpotLights;
	std::vector<Light>			*m_PointLights;
	std::vector<Light>			*m_DirectionalLights;
	unsigned int				m_MaxLightsPerLightInstance;

	DirectX::XMFLOAT3			*m_CameraPosition;
	DirectX::XMFLOAT4X4			*m_ViewMatrix;
	DirectX::XMFLOAT4X4			*m_ProjectionMatrix;

	static const unsigned int	m_numRenderTargets = 5;
	ID3D11RenderTargetView		*m_RenderTargets[m_numRenderTargets];

	ID3D11ShaderResourceView	*m_DiffuseSRV;
	ID3D11ShaderResourceView	*m_NormalSRV;
	ID3D11ShaderResourceView	*m_LightSRV;
	ID3D11ShaderResourceView	*m_wPositionSRV;
	ID3D11ShaderResourceView	*m_SSAO_SRV;

	ID3D11SamplerState			*m_Sampler;
	ID3D11BlendState			*m_BlendState;
	ID3D11BlendState			*m_BlendState2;
	Buffer						*m_AnimatedObjectConstantBuffer;
	Buffer						*m_WorldInstanceData;
	Shader						*m_InstancedGeometryShader;

	ID3D11RasterizerState		*m_RasterState;
	ID3D11DepthStencilState		*m_DepthState;

	Shader						*m_PointShader;
	Shader						*m_SpotShader;
	Shader						*m_DirectionalShader;

	Buffer						*m_PointModelBuffer;
	Buffer						*m_SpotModelBuffer;
	Buffer						*m_DirectionalModelBuffer;

	Buffer						*m_ConstantBuffer;
	Buffer						*m_ObjectConstantBuffer;
	Buffer						*m_AllLightBuffer;

	Buffer						*m_SkyDomeBuffer;
	Shader						*m_SkyDomeShader;
	ID3D11ShaderResourceView	*m_SkyDomeSRV;
	ID3D11DepthStencilState		*m_SkyDomeDepthStencilState;
	ID3D11RasterizerState		*m_SkyDomeRasterizerState;
	bool						m_RenderSkyDome;
	ID3D11SamplerState			*m_SkyDomeSampler;

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
		DirectX::XMFLOAT3 *p_CameraPosition, DirectX::XMFLOAT4X4 *p_ViewMatrix,
		DirectX::XMFLOAT4X4 *p_ProjectionMatrix, std::vector<Light> *p_SpotLights,
		std::vector<Light> *p_PointLights, std::vector<Light> *p_DirectionalLights,
		unsigned int p_MaxLightsPerLightInstance);

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
	ID3D11ShaderResourceView* getRT(int i); //DEBUG

private:
	void renderGeometry();

	void clearRenderTargets(unsigned int nrRT);

	void renderLighting();
	void renderSkyDomeImpl();

	void renderLight(Shader *p_Shader, Buffer *p_ModelBuffer, std::vector<Light> *p_Lights);

	void updateConstantBuffer();
	void updateLightBuffer();

	HRESULT createRenderTargets(D3D11_TEXTURE2D_DESC &desc);
	HRESULT createShaderResourceViews(D3D11_TEXTURE2D_DESC &desc);
	void createBuffers();
	void clearRenderTargets();
	void createSamplerState();
	void createBlendStates();
	void createLightShaders();
	void loadLightModels();
	void createLightStates(); //Rasterize and depth state
	void createRandomTexture(unsigned int p_Size);

	void renderObject(Renderable &p_Object);
};

