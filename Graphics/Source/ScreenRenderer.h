#pragma once
#include "Renderable2D.h"

class ScreenRenderer
{
private:
	ID3D11Device *m_Device;
	ID3D11DeviceContext	*m_DeviceContext;

	ID3D11DepthStencilView	*m_DepthStencilView;
	ID3D11RenderTargetView	*m_RenderTarget;
	ID3D11SamplerState *m_Sampler;
	ID3D11RasterizerState *m_RasterState;
	ID3D11DepthStencilState *m_DepthStencilState;

	DirectX::XMFLOAT4X4	*m_ViewMatrix;
	DirectX::XMFLOAT4X4	m_OrthoMatrix;


	std::vector<Renderable2D> m_2D_Objects;
	Buffer *m_ConstantBuffer;
	Buffer *m_ObjectConstantBuffer;

	Shader *m_HUD_Shader;

	ID3D11BlendState *m_TransparencyAdditiveBlend;

public:
	/**
	* Constructor.
	*/
	ScreenRenderer(void);

	/**
	* Destructor.
	*/
	~ScreenRenderer(void);

	/**
	* Initialize all the needed variables for rendering 2D elements.
	* @param p_Device, DirectX Device used for rendering
	* @param p_DeviceContect, DirectX device context. Used for rendering.
	* @param p_CameraPosition, the camera position. Used when rendering.
	* @param p_ViewMatrix, the view matrix. Used when rendering.
	* @param p_ProjectionMatrix, the projection matrix. Used when rendering.
	* @param p_DepthStencilView, used for z-culling when rendering.
	* @param p_RenderTarget, make it use the same render target as the other.
	*/
	void initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT4X4 *p_ViewMatrix,
		DirectX::XMFLOAT4 p_OrthoData, ID3D11DepthStencilView* p_DepthStencilView,
		ID3D11RenderTargetView *p_RenderTarget);

	void add2D_Object(Renderable2D &p_Object);

	/**
	* Call to render the graphics using forward rendering.
	* All the objects that are supposed to be rendered must have been sent to the renderer
	* before calling this function.
	*/
	void renderScreen(void);

private:
	void createBlendState(void);
	void createBuffers(void);
	void createSampler(void);
	void createRasterState(void);
	void createDepthStencilState(void);
	void updateConstantBuffer(void);
	void renderObject(Renderable2D &p_Object);
};

