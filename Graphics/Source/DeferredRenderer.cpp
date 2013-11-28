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

	/*srvt0 = nullptr;
	srvt1 = nullptr;
	srvt2 = nullptr;
	srvt3 = nullptr;*/
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
	
	// Make the Depth texture from the render target.
	dssrvdesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	m_RenderTargets[2]->GetResource(&tt);
	m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_DepthSRV);
	tt = nullptr;

	// Make the Light texture from the render target.
	dssrvdesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	m_RenderTargets[3]->GetResource(&tt);
	m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_LightSRV);
	tt = nullptr;

	//Create shader for the lightpass
	m_LightShader = new Shader();
	m_LightShader->initialize(m_Device,m_DeviceContext, 0);
	m_LightShader->compileAndCreateShader(L"../../Graphics/Source/DummyLightPass.hlsl","VSmain","vs_5_0",VERTEX_SHADER, nullptr);
	m_LightShader->compileAndCreateShader(L"../../Graphics/Source/DummyLightPass.hlsl","PSmain","ps_5_0",PIXEL_SHADER, nullptr);
	
	// TEMPORARY -----------------------------------------------------------
	// Make the light
	Light testLight(	
		DirectX::XMFLOAT3(10.0f, 20.0f, -20.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 
		DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), 
		DirectX::XMFLOAT2(1.0f, 1.0f),  
		100.0f,
		0
	);
	m_Lights.push_back(testLight);

	BufferDescription sbdesc;
	sbdesc.initData = &testLight;
	sbdesc.numOfElements = 1;
	sbdesc.sizeOfElement = sizeof(Light);
	sbdesc.type = CONSTANT_BUFFER_ALL;
	sbdesc.usage = BUFFER_DEFAULT;
	m_LightBuffer = new Buffer();
	m_LightBuffer->initialize(m_Device, m_DeviceContext, sbdesc);


	
	// Fundera på om vi ska använda meshes eller inte. Påverkar ifall vi behöver ha ett vertex-steg i light passet.

	// TEMPORARY -----------------------------------------------------------------
}

void DeferredRenderer::renderDeferred()
{
	renderGeometry();
	//renderLighting();
	renderFinal();
	renderForward();
	
	m_Objects.clear();
	m_TransparentObjects.clear();
}

void DeferredRenderer::renderGeometry()
{
	unsigned int nrRT = 3;
	m_DeviceContext->OMSetRenderTargets(nrRT, m_RenderTargets, m_DepthStencilView);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for(unsigned int i = 0; i < m_Objects.size();i++)
	{
		//Send stuff
		m_Objects.at(i).m_ConstantBuffer->setBuffer(1);
		// The update of the subresource has to be done externally.

		m_Objects.at(i).m_Buffer->setBuffer(0);
		//set shader
		m_Objects.at(i).m_Shader->setShader();

		m_DeviceContext->Draw(m_Objects.at(i).m_Buffer->getNumOfElements(), 0);

		m_Objects.at(i).m_Buffer->unsetBuffer(0);
		m_Objects.at(i).m_ConstantBuffer->unsetBuffer(1);
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
	unsigned int RTOffset	= 3;
	unsigned int nrRT		= 1;
	ID3D11ShaderResourceView *srvs[] = {m_DepthStencilResourceView,
										m_NormalSRV,
										m_DiffuseSRV};

	m_DeviceContext->VSSetShaderResources(0, 3, srvs);
	Buffer* p = m_Objects.at(0).m_ConstantBuffer;
	
	cBuffer cb;
	DirectX::XMFLOAT4 eye,lookat,up;
	eye = DirectX::XMFLOAT4(10,0,-50,1);
	lookat = DirectX::XMFLOAT4(0,0,0,1);
	up = DirectX::XMFLOAT4(0,1,0,0);
	DirectX::XMStoreFloat4x4(&cb.view,
							DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(
								DirectX::XMLoadFloat4(&eye),
								DirectX::XMLoadFloat4(&lookat),
								DirectX::XMLoadFloat4(&up))));
	DirectX::XMStoreFloat4x4(&cb.proj,
							DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(
								0.4f*3.14f,
								(float)800 / (float)480,
								1.0f,
								1000.0f)));

	DirectX::XMStoreFloat4x4(&cb.view, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&cb.view)));
	DirectX::XMStoreFloat4x4(&cb.proj, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&cb.proj)));
	cb.campos = DirectX::XMFLOAT3(eye.x, eye.y, eye.z);

	m_DeviceContext->UpdateSubresource(p->getBufferPointer(), 0,nullptr, &cb,0,0);
	
	//Select the third rendertarget[2]
	m_DeviceContext->OMSetRenderTargets(nrRT, m_RenderTargets + RTOffset, m_DepthStencilView); 
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	p->setBuffer(0);

	for(unsigned int i = 0; i < m_Lights.size();i++)
	{
		//Send stuff
		// m_PointLights.at(i).m_ConstantBuffer->setBuffer(0);
		// The update of the subresource has to be done externally.
		
		m_LightBuffer->setBuffer(1);
		
		//set shader
		m_LightShader->setShader();

		m_DeviceContext->Draw(6, 0);

		m_LightBuffer->unsetBuffer(1);
		
		//m_ConstantBuffer->unsetBuffer(0);
		m_LightShader->unSetShader();
	}
	p->unsetBuffer(0);
	float color[4] = {0.0f, 0.5f, 0.0f, 1.0f};
	for( unsigned int i = 0 + RTOffset; i < nrRT + RTOffset; i++)
	{
		m_DeviceContext->ClearRenderTargetView(m_RenderTargets[i], color);
	}
	

	eye = DirectX::XMFLOAT4(10,0,-50,1);
	lookat = DirectX::XMFLOAT4(0,0,0,1);
	up = DirectX::XMFLOAT4(0,1,0,0);
	DirectX::XMStoreFloat4x4(&cb.view,
							DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(
								DirectX::XMLoadFloat4(&eye),
								DirectX::XMLoadFloat4(&lookat),
								DirectX::XMLoadFloat4(&up))));
	DirectX::XMStoreFloat4x4(&cb.proj,
							DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(
								0.4f*3.14f,
								(float)800 / (float)480,
								1.0f,
								1000.0f)));

	cb.campos = DirectX::XMFLOAT3(eye.x, eye.y, eye.z);

	m_DeviceContext->UpdateSubresource(p->getBufferPointer(), 0,nullptr, &cb,0,0);
	p = nullptr;
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

ID3D11ShaderResourceView* DeferredRenderer::getRT(int i)
{
	switch(i)
	{
		case 0: return m_DiffuseSRV;
		case 1: return m_NormalSRV;
		case 2: return m_LightSRV;
		case 3: return m_DepthSRV;
	}
}