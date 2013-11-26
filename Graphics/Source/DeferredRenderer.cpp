#include "DeferredRenderer.h"


DeferredRenderer::DeferredRenderer(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_DepthStencilView = nullptr;
	for(int i = 0; i < 4; i++)
	{
		m_RenderTargets[i] = nullptr;
	}
}


DeferredRenderer::~DeferredRenderer(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_DepthStencilView = nullptr;

	for(int i = 0; i < 4; i++)
	{
		SAFE_RELEASE(m_RenderTargets[i]);
	}

	m_Objects.clear();
	m_TransparentObjects.clear();
}

void DeferredRenderer::shutdown()
{

}

void DeferredRenderer::initialize(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext,
								  ID3D11DepthStencilView *p_DepthStencilView,
									unsigned int p_screenWidth, unsigned int p_screenHeight)
{
	m_Device = p_Device;
	m_DeviceContext = p_DeviceContext;
	m_DepthStencilView = p_DepthStencilView;

	// Create the render target texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.Width = p_screenWidth;
	desc.Height = p_screenHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	//Create the render targets
	ID3D11Texture2D *srvt0 = nullptr;
	ID3D11Texture2D *srvt1 = nullptr;
	ID3D11Texture2D *srvt2 = nullptr;
	ID3D11Texture2D *srvt3 = nullptr;
	m_Device->CreateTexture2D(&desc, nullptr, &srvt0);
	m_Device->CreateTexture2D(&desc, nullptr, &srvt1);
	m_Device->CreateTexture2D(&desc, nullptr, &srvt2);
	m_Device->CreateTexture2D(&desc, nullptr, &srvt3);

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	m_Device->CreateRenderTargetView(srvt0, &rtDesc, &m_RenderTargets[0]);
	m_Device->CreateRenderTargetView(srvt1, &rtDesc, &m_RenderTargets[1]);
	m_Device->CreateRenderTargetView(srvt2, &rtDesc, &m_RenderTargets[2]);
	m_Device->CreateRenderTargetView(srvt3, &rtDesc, &m_RenderTargets[3]);

	srvt0 = nullptr;
	srvt1 = nullptr;
	srvt2 = nullptr;
	srvt3 = nullptr;
	//Done with the render targets
}

void DeferredRenderer::renderDeferred()
{
	renderGeometry();
	renderLighting();
	renderFinal();
	renderForward();
	
	m_Objects.clear();
	m_TransparentObjects.clear();
}

void DeferredRenderer::renderGeometry()
{
	m_DeviceContext->OMSetRenderTargets(1, m_RenderTargets, m_DepthStencilView);

	for(int i = 0; i < m_Objects.size();i++)
	{
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		

		//Send stuff
		m_Objects.at(i).m_ConstantBuffer->setBuffer(0);
		// The update of the subresource has to be done externally.

		m_Objects.at(i).m_Buffer->setBuffer(0);
		//set shader
		m_Objects.at(i).m_Shader->setShader();


		m_DeviceContext->Draw(m_Objects.at(i).m_Buffer->getNumOfElements(), 0);

		m_Objects.at(i).m_Buffer->unsetBuffer(0);
		m_Objects.at(i).m_ConstantBuffer->unsetBuffer(0);
		m_Objects.at(i).m_Shader->unSetShader();
	}

	float color[4] = {0.0f, 0.5f, 0.0f, 1.0f};
	for(int i = 0; i < 1; i++)
	{
		m_DeviceContext->ClearRenderTargetView(m_RenderTargets[i], color);
	}
}

void DeferredRenderer::renderLighting()
{

}

void DeferredRenderer::renderFinal()
{

}

void DeferredRenderer::renderForward()
{

}

void DeferredRenderer::addRenderable(Renderable p_renderable, bool p_Transparent)
{
	std::vector<Renderable>* temp;
	if(p_Transparent)
	{
		temp = &m_TransparentObjects;
	}
	else
	{
		temp = &m_Objects;
	}
	temp->push_back(p_renderable);
	temp = nullptr;
}

void DeferredRenderer::addLight()
{

}