#pragma once
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <memory>
#include <vector>
//#include "WrapperFactory.h"
#include "Util.h"
#include <DirectXMath.h>
#include "LightStructs.h"
#include "TextureLoader.h"
#include "ModelDefinition.h"
#include "VRAMMemInfo.h"
#include "ModelBinaryLoader.h"



/*
 * cBuffer contains the matrices needed to render the models and lights.
 */
struct cBuffer
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT3	campos;
};

struct cObjectBuffer
{
	DirectX::XMFLOAT4X4 world;
};

struct cAnimatedObjectBuffer
{
	DirectX::XMFLOAT4X4 invTransposeWorld;
	DirectX::XMFLOAT4X4 boneTransform[96];
};

class DeferredRenderer
{
public:
	/*
	 * Renderable is a debug struct made with the only purpose to be a placeholder for models
	 * until the model loader is done.
	 * ### The inverse transpose world matrix is needed to render animations and is not stored 
	 * ### anywhere else than here. Remember to move it if this struct is deleted.
	 */
	struct Renderable
	{
		ModelDefinition				*m_Model;
		DirectX::XMFLOAT4X4			m_World;
		DirectX::XMFLOAT4X4			m_invTransposeWorld;
		const std::vector<DirectX::XMFLOAT4X4> *m_FinalTransforms;

		Renderable(ModelDefinition *p_Model, const DirectX::XMFLOAT4X4& p_World, const std::vector<DirectX::XMFLOAT4X4>* p_FinalTransforms = nullptr)
		{
			m_Model = p_Model;
			m_World = p_World;

			using namespace DirectX;

			XMStoreFloat4x4( &m_invTransposeWorld, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_World))) ); 

			m_FinalTransforms = p_FinalTransforms;
		}

		~Renderable()
		{
			m_Model = nullptr;
		}
	};
private:
	std::vector<Renderable>		m_Objects;

	ID3D11Device				*m_Device;
	ID3D11DeviceContext			*m_DeviceContext;
	ID3D11DepthStencilView		*m_DepthStencilView;

	std::vector<Light>			*m_SpotLights;
	std::vector<Light>			*m_PointLights;
	std::vector<Light>			*m_DirectionalLights;
	static const unsigned int	m_MaxLightsPerLightInstance;

	DirectX::XMFLOAT3			*m_CameraPosition;
	DirectX::XMFLOAT4X4			*m_ViewMatrix;
	DirectX::XMFLOAT4X4			*m_ProjectionMatrix;

	static const unsigned int	m_numRenderTargets = 4;
	ID3D11RenderTargetView		*m_RenderTargets[m_numRenderTargets];

	ID3D11ShaderResourceView	*m_DiffuseSRV;
	ID3D11ShaderResourceView	*m_NormalSRV;
	ID3D11ShaderResourceView	*m_LightSRV;
	ID3D11ShaderResourceView	*m_wPositionSRV;

	ID3D11SamplerState			*m_Sampler;
	ID3D11BlendState			*m_BlendState;
	ID3D11BlendState			*m_BlendState2;
	Buffer						*m_AnimatedObjectConstantBuffer;

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

public:
	/*
	 * 
	 */
	DeferredRenderer();
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
		DirectX::XMFLOAT3 *p_CameraPosition, DirectX::XMFLOAT4X4 *p_ViewMatrix,
		DirectX::XMFLOAT4X4 *p_ProjectionMatrix, std::vector<Light> *p_SpotLights,
		std::vector<Light> *p_PointLights, std::vector<Light> *p_DirectionalLights);

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
	 * Use to get specific render targets to put on the back buffer.
	 * @ i, a number that is associated with a render target view.
	 * @return, render target if i is a legal number, else nullptr.
	 */
	ID3D11ShaderResourceView* getRT(int i); //DEBUG

private:
	void renderGeometry();

	void clearRenderTargets( unsigned int nrRT );

	void renderLighting();

	void renderLight(Shader *p_Shader, Buffer *p_ModelBuffer, vector<Light> *p_Lights);

	void updateConstantBuffer();
	void updateLightBuffer();

	HRESULT createRenderTargets(D3D11_TEXTURE2D_DESC &desc);
	HRESULT createShaderResourceViews( D3D11_TEXTURE2D_DESC &desc );
	void createBuffers();
	void clearRenderTargets();
	void createSamplerState();
	void createBlendStates();
	void createLightShaders();
	void loadLightModels();
	void createLightStates(); //Rasterize and depth state
};

