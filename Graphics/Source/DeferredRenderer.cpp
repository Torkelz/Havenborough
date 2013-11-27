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

	for(int i = 0; i < 3; i++)
	{
		SAFE_RELEASE(m_RenderTargets[i]);
	}

	m_Objects.clear();
	m_TransparentObjects.clear();
	SAFE_RELEASE(m_DepthStencilResourceView);
	SAFE_RELEASE(m_DiffuseSRV);
	SAFE_RELEASE(m_NormalSRV);
	SAFE_RELEASE(m_LightSRV);

	SAFE_DELETE(m_LightShader);
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
	m_Device->CreateTexture2D(&desc, nullptr, &srvt0);
	m_Device->CreateTexture2D(&desc, nullptr, &srvt1);
	m_Device->CreateTexture2D(&desc, nullptr, &srvt2);

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	m_Device->CreateRenderTargetView(srvt0, &rtDesc, &m_RenderTargets[0]);
	m_Device->CreateRenderTargetView(srvt1, &rtDesc, &m_RenderTargets[1]);
	m_Device->CreateRenderTargetView(srvt1, &rtDesc, &m_RenderTargets[2]);

	srvt0 = nullptr;
	srvt1 = nullptr;
	srvt2 = nullptr;
	//Done with the render targets

	// Make the depth stencil texture from the depth stencil view.
	D3D11_SHADER_RESOURCE_VIEW_DESC dssrvdesc;
	dssrvdesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	dssrvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	dssrvdesc.Texture2D.MipLevels = 1;
	dssrvdesc.Texture2D.MostDetailedMip = 0;

	ID3D11Resource* tt;
	p_DepthStencilView->GetResource(&tt);
	m_Device->CreateShaderResourceView(tt,&dssrvdesc, &m_DepthStencilResourceView);
	tt = nullptr;

	// Make the diffuse texture from the render target.
	dssrvdesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	m_RenderTargets[0]->GetResource(&tt);
	m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_DiffuseSRV);
	tt = nullptr;

	// Make the normal texture from the render target.
	dssrvdesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	m_RenderTargets[1]->GetResource(&tt);
	m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_NormalSRV);
	tt = nullptr;
	
	// Make the Light texture from the render target.
	dssrvdesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	m_RenderTargets[2]->GetResource(&tt);
	m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_LightSRV);
	tt = nullptr;

	//Create shader for the lightpass
	m_LightShader = new Shader();
	m_LightShader->initialize(m_Device,m_DeviceContext, 0);
	m_LightShader->compileAndCreateShader(L"../Source/DummyLightPass.hlsl","VS","vs_5_0",VERTEX_SHADER, nullptr);
	m_LightShader->compileAndCreateShader(L"../Source/DummyLightPass.hlsl","PS","ps_5_0",PIXEL_SHADER, nullptr);
	// TEMPORARY -----------------------------------------------------------
	// Make the light
	PointLight testLight(	
		DirectX::XMFLOAT4(0.0f, 15.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 
		100.0f
	);

	BufferDescription sbdesc;
	sbdesc.initData = nullptr;
	sbdesc.numOfElements = 1;
	sbdesc.sizeOfElement = sizeof(PointLight);
	sbdesc.type = STRUCTURED_BUFFER;
	sbdesc.usage = BUFFER_DEFAULT;
	m_LightBuffer = new Buffer();
	m_LightBuffer->initializeEx(m_Device, m_DeviceContext, sbdesc,true,true);


	m_PointLights.push_back(testLight);
	// Fundera på om vi ska använda meshes eller inte. Påverkar ifall vi behöver ha ett vertex-steg i light passet.

	// TEMPORARY -----------------------------------------------------------------
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
	unsigned int nrRT = 2;
	m_DeviceContext->OMSetRenderTargets(nrRT, m_RenderTargets, m_DepthStencilView);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for(unsigned int i = 0; i < m_Objects.size();i++)
	{
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
	for(unsigned int i = 0; i < nrRT; i++)
	{
		m_DeviceContext->ClearRenderTargetView(m_RenderTargets[i], color);
	}
}

void DeferredRenderer::renderLighting()
{
	unsigned int RTOffset	= 2;
	unsigned int nrRT		= 1;
	ID3D11ShaderResourceView *srvs[] = {m_DepthStencilResourceView,
										m_NormalSRV};

	m_DeviceContext->PSSetShaderResources(0, 2, srvs);
	
	m_DeviceContext->OMSetRenderTargets(nrRT, m_RenderTargets + RTOffset, m_DepthStencilView); //Select the third rendertarget[2]
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for(unsigned int i = 0; i < m_Objects.size();i++)
	{
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
	for( unsigned int i = 0 + RTOffset; i < nrRT + RTOffset; i++)
	{
		m_DeviceContext->ClearRenderTargetView(m_RenderTargets[i], color);
	}
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