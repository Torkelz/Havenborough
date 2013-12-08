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
#include "ModelLoader.h"

/*
 * Renderable is a debug struct made with the only purpose to be a placeholder for models
 * until the model loader is done.
 */
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
	ID3D11ShaderResourceView	*m_NormalSRV;
	ID3D11ShaderResourceView	*m_LightSRV;
	ID3D11ShaderResourceView	*m_wPositionSRV;
	ID3D11ShaderResourceView	*m_lightBufferSRV;

	ID3D11RasterizerState		*m_RasterState;
	ID3D11DepthStencilState		*m_DepthState;

	ID3D11SamplerState			*m_Sampler;
	ID3D11BlendState			*m_BlendState;
	ID3D11BlendState			*m_BlendState2;
	Shader						*m_LightShader;
	Buffer						*m_ConstantBuffer;
	Buffer						*m_AllLightBuffer;
	Buffer						*m_PointLightBuffer;

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

	/*
	 * Initialize all the needed variables for rendering.
	 * @ p_Device, DirectX Device used for rendering
	 * @ p_DeviceContect, DX device context. Used for rendering.
	 * @ p_DepthStencilView, used for z-culling when rendering.
	 * @ p_ScreenWidth, used to initialize render textures.
	 * @ p_ScreenHeight, used to initialize render textures.
	 * @ p_CameraPosition, pointer to the camera position. Used when rendering.
	 * @ p_ViewMatrix, pointer to the view matrix. Used when rendering.
	 * @ p_ProjectionMatrix, pointer to the projection matrix. Used when rendering.
	 */
	void initialize(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext,
		ID3D11DepthStencilView *p_DepthStencilView,
		unsigned int p_ScreenWidth, unsigned int p_ScreenHeight,
		DirectX::XMFLOAT3 *p_CameraPosition, DirectX::XMFLOAT4X4 *p_ViewMatrix,
		DirectX::XMFLOAT4X4 *p_ProjectionMatrix);

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

	std::vector<DirectX::XMFLOAT3> loadModelFromOBJFile( std::string p_OBJFileName );
};

