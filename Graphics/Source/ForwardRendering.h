#pragma once
#include "Renderable.h"

class ForwardRendering
{
private:
	ID3D11Device		*m_Device;
	ID3D11DeviceContext	*m_DeviceContext;

	ID3D11DepthStencilView	*m_DepthStencilView;
	ID3D11RenderTargetView	*m_RenderTarget;
	ID3D11SamplerState		*m_Sampler;
	ID3D11RasterizerState	*m_RasterState;
	ID3D11DepthStencilState *m_DepthStencilState;

	DirectX::XMFLOAT3	*m_CameraPosition;
	DirectX::XMFLOAT4X4	*m_ViewMatrix;
	DirectX::XMFLOAT4X4	*m_ProjectionMatrix;

	std::vector<Renderable> m_TransparencyObjects;
	Buffer					*m_ConstantBuffer;
	Buffer					*m_ObjectConstantBuffer;
	Buffer					*m_AnimatedObjectConstantBuffer;
	Buffer					*m_ColorShadingConstantBuffer;

	ID3D11BlendState		*m_TransparencyAdditiveBlend;

public:
	ForwardRendering(void);
	~ForwardRendering(void);

	/*
	 * Initialize all the needed variables for rendering.
	 *
	 * @ p_Device, DirectX Device used for rendering
	 * @ p_DeviceContect, DX device context. Used for rendering.
	 * @ p_CameraPosition, the camera position. Used when rendering.
	 * @ p_ViewMatrix, the view matrix. Used when rendering.
	 * @ p_ProjectionMatrix, the projection matrix. Used when rendering.
	 * @ p_DepthStencilView, used for z-culling when rendering.
	 * @ p_RenderTarget, make it use the same render target as the other.
	 */
	void init(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext,
		DirectX::XMFLOAT3 *p_CameraPosition, DirectX::XMFLOAT4X4 *p_ViewMatrix,
		DirectX::XMFLOAT4X4 *p_ProjectionMatrix, ID3D11DepthStencilView* p_DepthStencilView,
		ID3D11RenderTargetView *p_RenderTarget);

	/*
	 * Add models to the list of objects to be rendered with deferred rendering.
	 *
	 * @ p_Renderable, the model that needs to be rendered.
	 */
	void addRenderable(Renderable p_Renderable);

	/*
	 * Call to render the graphics using forward rendering.
	 *
	 * All the objects that are supposed to be rendered must have been sent to the renderer
	 * before calling this function.
	 */
	void renderForward();

private:
	void createBlendStates();
	void createForwardBuffers();
	void createSampler();
	void createRasterState();
	void createDepthStencilState(void);
	void updateConstantBuffer();
	bool depthSortCompareFunc(const Renderable &a, const Renderable &b);
	void renderObject(Renderable& p_Object);
};

