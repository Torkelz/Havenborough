#include "DeferredRenderer.h"


DeferredRenderer::DeferredRenderer(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;

	m_SRV_RT0 = nullptr;
	m_SRV_RT1 = nullptr;
	m_SRV_RT2 = nullptr;
	m_SRV_RT3 = nullptr;
}


DeferredRenderer::~DeferredRenderer(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	SAFE_RELEASE(m_SRV_RT0);
	SAFE_RELEASE(m_SRV_RT1);
	SAFE_RELEASE(m_SRV_RT2);
	SAFE_RELEASE(m_SRV_RT3);

	m_Objects.clear();
	m_TransparentObjects.clear();
}

void DeferredRenderer::shutdown()
{

}

void DeferredRenderer::initialize(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext,
									unsigned int p_screenWidth, unsigned int p_screenHeight)
{
	m_Device = p_Device;
	m_DeviceContext = p_DeviceContext;

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

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	m_Device->CreateShaderResourceView(srvt0, &srvDesc, &m_SRV_RT0);
	m_Device->CreateShaderResourceView(srvt1, &srvDesc, &m_SRV_RT1);
	m_Device->CreateShaderResourceView(srvt2, &srvDesc, &m_SRV_RT2);
	m_Device->CreateShaderResourceView(srvt3, &srvDesc, &m_SRV_RT3);

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
}

void DeferredRenderer::renderGeometry()
{
	for(int i = 0; i < m_Objects.size();i++)
	{
		//Send stuff
		
		//set shader
		m_Objects.at(i).m_Shader->setShader();

		//m_DeviceContext->Draw(m_Objects.at(i).m_Model->getBufferSize(), 0);

		m_Objects.at(i).m_Shader->unSetShader();
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