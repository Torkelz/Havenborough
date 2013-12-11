#include "DeferredRenderer.h"

DeferredRenderer::DeferredRenderer(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_DepthStencilView = nullptr;

	for(int i = 0; i < m_numRenderTargets; i++)
	{
		m_RenderTargets[i] = nullptr;
	}

	m_DiffuseSRV = nullptr;
	m_NormalSRV = nullptr;
	m_LightSRV = nullptr;
	m_wPositionSRV = nullptr;

	m_Sampler = nullptr;
	m_PointShader = nullptr;
	m_SpotShader = nullptr;
	m_DirectionalShader = nullptr;

	m_PointModelBuffer = nullptr;
	m_SpotModelBuffer = nullptr;

	m_ConstantBuffer = nullptr;
	m_AllLightBuffer = nullptr;
	m_ViewMatrix = nullptr;
	m_ProjectionMatrix = nullptr;
	m_CameraPosition = nullptr;

	m_RasterState = nullptr;
	m_DepthState = nullptr;

	m_speed = 1.0f;
}

DeferredRenderer::~DeferredRenderer(void)
{
	m_Objects.clear();
	m_PointLights.clear();
	m_SpotLights.clear();
	m_DirectionalLights.clear();

	m_Objects.shrink_to_fit();
	m_PointLights.shrink_to_fit();
	m_SpotLights.shrink_to_fit();
	m_DirectionalLights.shrink_to_fit();

	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_DepthStencilView = nullptr;

	m_ViewMatrix = nullptr;
	m_ProjectionMatrix = nullptr;
	m_CameraPosition = nullptr;

	for(int i = 0; i < m_numRenderTargets; i++)
	{
		SAFE_RELEASE(m_RenderTargets[i]);
	}

	SAFE_RELEASE(m_DiffuseSRV);
	SAFE_RELEASE(m_NormalSRV);
	SAFE_RELEASE(m_LightSRV);
	SAFE_RELEASE(m_wPositionSRV);

	SAFE_RELEASE(m_Sampler);
	SAFE_RELEASE(m_BlendState);
	SAFE_RELEASE(m_BlendState2);

	SAFE_RELEASE(m_RasterState);
	SAFE_RELEASE(m_DepthState);

	SAFE_DELETE(m_PointShader);
	SAFE_DELETE(m_SpotShader);
	SAFE_DELETE(m_DirectionalShader);

	SAFE_DELETE(m_PointModelBuffer);
	SAFE_DELETE(m_SpotModelBuffer);

	SAFE_DELETE(m_ConstantBuffer);
	SAFE_DELETE(m_ObjectConstantBuffer);
	SAFE_DELETE(m_AllLightBuffer);	
}

void DeferredRenderer::initialize(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext,
								  ID3D11DepthStencilView *p_DepthStencilView,
								  unsigned int p_screenWidth, unsigned int p_screenHeight,
								  DirectX::XMFLOAT3 *p_CameraPosition, DirectX::XMFLOAT4X4 *p_ViewMatrix,
								  DirectX::XMFLOAT4X4 *p_ProjectionMatrix)
{
	m_Device			= p_Device;
	m_DeviceContext		= p_DeviceContext;
	m_DepthStencilView	= p_DepthStencilView;

	m_CameraPosition	= p_CameraPosition;
	m_ViewMatrix		= p_ViewMatrix;
	m_ProjectionMatrix	= p_ProjectionMatrix;

	//Create render targets with the size of screen width and screen height
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.Width				= p_screenWidth;
	desc.Height				= p_screenHeight;
	desc.MipLevels			= 1;
	desc.ArraySize			= 1;
	desc.Format				= DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count	= 1;
	desc.Usage				= D3D11_USAGE_DEFAULT;
	desc.BindFlags			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	createRenderTargets(desc);

	createShaderResourceViews(desc);

	createLightShaders();

	loadLightModels();

	// Create sampler state and blend state for Alpha rendering.
	createSamplerState();
	createBlendStates();

	// TEMPORARY -----------------------------------------------------------
	// Make the light
	Light testLight(	
		DirectX::XMFLOAT3(0.0f, 20.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 
		DirectX::XMFLOAT3(0.01f, -0.99f, 0.0f), 
		DirectX::XMFLOAT2(cosf(3.14f/16.f),cosf(3.14f/7.f)),  
		70.0f,
		1
		);
	//addLight(testLight);

	xx = 3;
	yy = 1;
	zz = 3;
	int minX,minY,minZ,maxX,maxY,maxZ;
	minX = minY = minZ = -30;
	maxX = maxY = maxZ = 30;
	minY = 20;
	maxY = 50;
	float dx,dy,dz;
	dx = (float)(abs(maxX) + abs(minX))/((xx - 1 <= 0) ? 1 : xx - 1);
	dy = (float)(abs(maxY) + abs(minY))/((yy - 1 <= 0) ? 1 : xx - 1);
	dz = (float)(abs(maxZ) + abs(minZ))/((zz - 1 <= 0) ? 1 : xx - 1);

	for(int x= 0; x < xx; x++)
	{
		for(int y = 0; y < yy; y++)
		{
			for(int z = 0; z < zz; z++)
			{
				testLight.lightPos.x = (x * dx) + minX;
				testLight.lightPos.y = (y * dy) + minY;
				testLight.lightPos.z = (z * dz) + minZ;

				addLight(testLight);
			}
		}
	}
	// TEMPORARY ----------------------------------------------------------------
	createLightStates();

	Buffer::Description cbdesc;
	cbdesc.initData = nullptr;//m_Lights.data();
	cbdesc.numOfElements = 200;
	cbdesc.sizeOfElement = sizeof(Light);
	cbdesc.type = Buffer::Type::VERTEX_BUFFER;
	cbdesc.usage = Buffer::Usage::DEFAULT;
	m_AllLightBuffer = WrapperFactory::getInstance()->createBuffer(cbdesc);

	createConstantBuffer();
}

void DeferredRenderer::renderDeferred()
{
	/*float dt = 0.016f;
	for(unsigned int i = 0; i < m_Lights.size();i++)
	{
	
		if(i%2 == 0)
			m_Lights.at(i).lightPos.y += m_speed * dt;
		else
			m_Lights.at(i).lightPos.y -= m_speed * dt;	
	}
	if((m_Lights.at(0).lightPos.y > 30) ||
		(m_Lights.at(0).lightPos.y < -30))
		m_speed *= -1.0f;*/


	// Clear render targets.
	clearRenderTargets();

	//// Update constant buffer
	//updateConstantBuffer(xx*yy*zz);
	//updateLightBuffer();

	// Render
	renderGeometry();

	// Update constant buffer. ## REMOVE WHEN NINJA KICK IS REMOVED. ##
	updateConstantBuffer();
	updateLightBuffer();

	renderLighting();

	m_Objects.clear();
	//m_Lights.clear(); For debug this is not active
}

void DeferredRenderer::renderGeometry()
{
	unsigned int nrRT = 3;

	// Set the render targets.
	m_DeviceContext->OMSetRenderTargets(nrRT, m_RenderTargets, m_DepthStencilView);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// The textures will be needed to be grabbed from the model later.
	ID3D11ShaderResourceView *nullsrvs[] = {0,0,0};

	m_ConstantBuffer->setBuffer(1);
	m_DeviceContext->PSSetSamplers(0,1,&m_Sampler);
	updateConstantBuffer();
	for(unsigned int i = 0; i < m_Objects.size();i++)
	{
		m_Objects.at(i).m_Model->vertexBuffer->setBuffer(0);
		for(unsigned int j = 0; j < m_Objects.at(i).m_Model->numOfMaterials;j++)
		{
			ID3D11ShaderResourceView *srvs[] =  {	m_Objects.at(i).m_Model->diffuseTexture[j], 
													m_Objects.at(i).m_Model->normalTexture[j], 
													m_Objects.at(i).m_Model->specularTexture[j] 
												};
			m_DeviceContext->PSSetShaderResources(0, 3, srvs);
			
			// Send stuff.
			// The update of the sub resource has to be done externally.
			
			m_Objects.at(i).m_Model->indexBuffer[j]->setBuffer(0);
			cObjectBuffer temp;
			temp.world = *m_Objects.at(i).m_World;
			m_DeviceContext->UpdateSubresource(m_ObjectConstantBuffer->getBufferPointer(), NULL,NULL, &temp,NULL,NULL);
			m_ObjectConstantBuffer->setBuffer(2);

			// Set shader.
			m_Objects.at(i).m_Model->shader->setShader();
			float data[] = { 1.0f, 1.0f, 1.f, 1.0f};
			m_Objects.at(i).m_Model->shader->setBlendState(m_BlendState2, data);

			//m_DeviceContext->Draw(m_Objects.at(i).m_Model->vertexBuffer->getNumOfElements(), 0);
			m_DeviceContext->DrawIndexed(m_Objects.at(i).m_Model->indexBuffer[j]->getNumOfElements(),0, 0);

			m_Objects.at(i).m_Model->vertexBuffer->unsetBuffer(0);
			m_Objects.at(i).m_Model->indexBuffer[j]->unsetBuffer(0);
			m_ObjectConstantBuffer->unsetBuffer(2);
			m_Objects.at(i).m_Model->shader->setBlendState(0, data);
			m_Objects.at(i).m_Model->shader->unSetShader();
		}
	}

	m_DeviceContext->PSSetShaderResources(0, 3, nullsrvs);
	m_DeviceContext->PSSetSamplers(0,0,0);
	m_ConstantBuffer->unsetBuffer(1);

	// Unset render targets.
	m_DeviceContext->OMSetRenderTargets(0, 0, 0);
}

void DeferredRenderer::renderLighting()
{
	unsigned int activeRenderTarget	= 3;
	unsigned int nrRT = 1;

	// Store previous States to be set when we exit the method.
	ID3D11RasterizerState *previousRasterState;
	ID3D11DepthStencilState *previousDepthState;
	m_DeviceContext->RSGetState(&previousRasterState);
	m_DeviceContext->OMGetDepthStencilState(&previousDepthState,0);

	// Collect the shader resources in an array and create a clear array.
	ID3D11ShaderResourceView *srvs[] = {m_wPositionSRV, m_NormalSRV, m_DiffuseSRV};
	ID3D11ShaderResourceView *nullsrvs[] = {0,0,0};

	// Set texture sampler.
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	UINT sampleMask = 0xffffffff;
	m_DeviceContext->PSSetShaderResources(0, 3, srvs);

	////Select the third render target[3]
	m_DeviceContext->OMSetRenderTargets(nrRT, &m_RenderTargets[activeRenderTarget], m_DepthStencilView); 
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_DeviceContext->RSSetState(m_RasterState);
	m_DeviceContext->OMSetDepthStencilState(m_DepthState,0);
	m_DeviceContext->OMSetBlendState(m_BlendState, blendFactor, sampleMask);

	//Set constant data
	m_ConstantBuffer->setBuffer(0);


	//		Render PointLights
	renderLight(m_PointShader, m_PointModelBuffer, &m_PointLights);
	//		Render SpotLights
	renderLight(m_SpotShader, m_SpotModelBuffer, &m_SpotLights);
	//		Render DirectionalLights
	//RenderLight(m_PointModelBuffer, &m_DirectionalLights);


	m_ConstantBuffer->unsetBuffer(0);

	m_DeviceContext->PSSetShaderResources(0, 3, nullsrvs);
	m_DeviceContext->OMSetRenderTargets(0, 0, 0);
	m_DeviceContext->RSSetState(previousRasterState);
	m_DeviceContext->OMSetDepthStencilState(previousDepthState,0);
	m_DeviceContext->OMSetBlendState(0, blendFactor, sampleMask);
}

void DeferredRenderer::addRenderable(Renderable p_renderable)
{
	m_Objects.push_back(p_renderable);
}

void DeferredRenderer::addLight(Light p_Light)
{
	switch (p_Light.lightType)
	{
	case 0:
		m_PointLights.push_back(p_Light);
		break;
	case 1:
		m_SpotLights.push_back(p_Light);
		break;
	case 2:
		m_DirectionalLights.push_back(p_Light);
		break;
	default:
		break;
	}
}

ID3D11ShaderResourceView* DeferredRenderer::getRT(int i)
{
	switch(i)
	{
		case 0: return m_DiffuseSRV;
		case 1: return m_NormalSRV;
		case 2: return m_wPositionSRV;
		case 3: return m_LightSRV;
		default: return nullptr;
	}
}

void DeferredRenderer::debugUpdateCone(float yaw, float pitch)
{
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.f);
	DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0.f, 0.f, -1.f));

	DirectX::XMStoreFloat3(&m_SpotLights[0].lightDirection, DirectX::XMVector3Transform(dir, rotMat));
}

void DeferredRenderer::updateConstantBuffer()
{
	cBuffer cb;
	cb.view = *m_ViewMatrix;
	cb.proj = *m_ProjectionMatrix;
	cb.campos = *m_CameraPosition;
	m_DeviceContext->UpdateSubresource(m_ConstantBuffer->getBufferPointer(), NULL,NULL, &cb,NULL,NULL);
}

void DeferredRenderer::updateLightBuffer()
{
	//m_DeviceContext->UpdateSubresource(m_AllLightBuffer->getBufferPointer(), NULL,NULL, m_Lights.data(),NULL,NULL);
}

HRESULT DeferredRenderer::createRenderTargets(D3D11_TEXTURE2D_DESC &desc)
{
	// Create the render target texture
	HRESULT result = S_FALSE;

	//Create the render targets
	ID3D11Texture2D *srvt0, *srvt1, *srvt2, *srvt3;
	srvt0 = srvt1 = srvt2 = srvt3 = nullptr;

	result = m_Device->CreateTexture2D(&desc, nullptr, &srvt0);
	if(FAILED(result))
		return result;
	result = m_Device->CreateTexture2D(&desc, nullptr, &srvt1);
	if(FAILED(result))
		return result;
	result = m_Device->CreateTexture2D(&desc, nullptr, &srvt2);
	if(FAILED(result))
		return result;
	result = m_Device->CreateTexture2D(&desc, nullptr, &srvt3);
	if(FAILED(result))
		return result;

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	result = m_Device->CreateRenderTargetView(srvt0, &rtDesc, &m_RenderTargets[0]);
	if(FAILED(result))
		return result;
	result = m_Device->CreateRenderTargetView(srvt1, &rtDesc, &m_RenderTargets[1]);
	if(FAILED(result))
		return result;
	result = m_Device->CreateRenderTargetView(srvt2, &rtDesc, &m_RenderTargets[2]);
	if(FAILED(result))
		return result;
	result = m_Device->CreateRenderTargetView(srvt3, &rtDesc, &m_RenderTargets[3]);
	if(FAILED(result))
		return result;
	SAFE_RELEASE(srvt0);
	SAFE_RELEASE(srvt1);
	SAFE_RELEASE(srvt2);
	SAFE_RELEASE(srvt3);
	// Done with the render targets.

	unsigned int size = 4 * VRAMMemInfo::getInstance()->calculateFormatUsage(desc.Format, desc.Width, desc.Height);
	VRAMMemInfo::getInstance()->updateUsage(size);

	return result;
}

HRESULT DeferredRenderer::createShaderResourceViews( D3D11_TEXTURE2D_DESC &desc )
{
	HRESULT result = E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC dssrvdesc;
	dssrvdesc.Format = dssrvdesc.Format = desc.Format;
	dssrvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	dssrvdesc.Texture2D.MipLevels = 1;
	dssrvdesc.Texture2D.MostDetailedMip = 0;

	ID3D11Resource* tt;

	// Make the diffuse texture from the render target.	
	m_RenderTargets[0]->GetResource(&tt);
	result = m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_DiffuseSRV);
	tt = nullptr;
	if(FAILED(result))
		return result;

	// Make the normal texture from the render target.
	m_RenderTargets[1]->GetResource(&tt);
	result = m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_NormalSRV);
	tt = nullptr;
	if(FAILED(result))
		return result;

	// Make the world position texture from the render target.
	m_RenderTargets[2]->GetResource(&tt);
	result = m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_wPositionSRV);
	tt = nullptr;
	if(FAILED(result))
		return result;

	// Make the final texture from the render target.
	m_RenderTargets[3]->GetResource(&tt);
	result = m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_LightSRV);
	tt = nullptr;
	if(FAILED(result))
		return result;

	return result;
}

void DeferredRenderer::createConstantBuffer()
{
	cBuffer cb;
	cb.view = *m_ViewMatrix;
	cb.proj = *m_ProjectionMatrix;
	cb.campos = *m_CameraPosition;

	Buffer::Description cbdesc;
	cbdesc.initData = &cb;
	cbdesc.numOfElements = 1;
	cbdesc.sizeOfElement = sizeof(cBuffer);
	cbdesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;
	cbdesc.usage = Buffer::Usage::DEFAULT;

	m_ConstantBuffer = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMMemInfo::getInstance()->updateUsage(sizeof(cBuffer));

	cbdesc.sizeOfElement = sizeof(cObjectBuffer);
	m_ObjectConstantBuffer = new Buffer();
	m_ObjectConstantBuffer->initialize(m_Device, m_DeviceContext, cbdesc);

	VRAMMemInfo::getInstance()->updateUsage(sizeof(cObjectBuffer));
}

void DeferredRenderer::clearRenderTargets()
{
	float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_DeviceContext->ClearRenderTargetView(m_RenderTargets[0], color);

	color[0] = color[1] = color[2] = 0.5f;
	m_DeviceContext->ClearRenderTargetView(m_RenderTargets[1], color);

	color[0] = color[1] = color[2] = 1.0f;
	m_DeviceContext->ClearRenderTargetView(m_RenderTargets[2], color);

	color[0] = color[1] = color[2] = 0.0f;
	m_DeviceContext->ClearRenderTargetView(m_RenderTargets[3], color);
}

void DeferredRenderer::createSamplerState()
{
	// Create texture sampler.
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc   = D3D11_COMPARISON_NEVER;
	sd.MinLOD			= 0;
	sd.MaxLOD			= D3D11_FLOAT32_MAX;

	m_Device->CreateSamplerState( &sd, &m_Sampler );
}

void DeferredRenderer::createBlendStates()
{
	D3D11_BLEND_DESC bd;
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend =  D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_Device->CreateBlendState(&bd, &m_BlendState);

	for(int i = 0; i < 4; i++)
	{
		bd.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		bd.RenderTarget[i].DestBlend =  D3D11_BLEND_ZERO;
		bd.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	}

	m_Device->CreateBlendState(&bd, &m_BlendState2);
}

void DeferredRenderer::createLightShaders()
{
	ShaderInputElementDescription shaderDesc[] = 
	{
		{"POSITION",	0, Format::R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,	  0},
		{"LPOSITION",	0, Format::R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA,  1},
		{"COLOR",		0, Format::R32G32B32_FLOAT, 1, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"DIRECTION",	0, Format::R32G32B32_FLOAT, 1, 24, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"ANGLE",		0, Format::R32G32_FLOAT,	1, 36, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"RANGE",		0, Format::R32_FLOAT,		1, 44, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TYPE",		0, Format::R32_UINT,		1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	m_SpotShader = WrapperFactory::getInstance()->createShader(L"../../Graphics/Source/DeferredShaders/LightPassSpotLight.hlsl",
								"SpotLightVS,SpotLightPS", "5_0",ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER, shaderDesc, 7);

	m_PointShader = WrapperFactory::getInstance()->createShader(L"../../Graphics/Source/DeferredShaders/LightPassPointLight.hlsl",
		"PointLightVS,PointLightPS", "5_0",ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER, shaderDesc, 7);
}

void DeferredRenderer::loadLightModels()
{
	ModelLoader modelLoader;
	modelLoader.loadFile("../../Graphics/Resources/spotlight +Z openGL 2.tx");
	const std::vector<std::vector<ModelLoader::IndexDesc>>& indices = modelLoader.getIndices();
	const std::vector<DirectX::XMFLOAT3>& vertices = modelLoader.getVertices();
	std::vector<DirectX::XMFLOAT3> temp;
	unsigned int nrIndices = indices.at(0).size();
	for(unsigned int i = 0; i < nrIndices; i++)
	{
		temp.push_back(vertices.at(indices.at(0).at(i).m_Vertex));
	}
	Buffer::Description cbdesc;
	cbdesc.initData = temp.data();
	cbdesc.numOfElements = nrIndices;
	cbdesc.sizeOfElement = sizeof(DirectX::XMFLOAT3);
	cbdesc.type = Buffer::Type::VERTEX_BUFFER;
	cbdesc.usage = Buffer::Usage::USAGE_IMMUTABLE;

	m_SpotModelBuffer = WrapperFactory::getInstance()->createBuffer(cbdesc);
	temp.clear();
	modelLoader.loadFile("../../Graphics/Resources/sphere.tx");
	const std::vector<std::vector<ModelLoader::IndexDesc>>& indices2 = modelLoader.getIndices();
	const std::vector<DirectX::XMFLOAT3>& vertices2 = modelLoader.getVertices();

	nrIndices = indices2.at(0).size();
	for(unsigned int i = 0; i < nrIndices; i++)
	{
		temp.push_back(vertices2.at(indices2.at(0).at(i).m_Vertex));
	}
	cbdesc.initData = temp.data();
	cbdesc.numOfElements = nrIndices;
	m_PointModelBuffer = WrapperFactory::getInstance()->createBuffer(cbdesc);
	temp.clear();
	temp.shrink_to_fit();

	modelLoader.clear();
}

void DeferredRenderer::createLightStates()
{
	D3D11_DEPTH_STENCIL_DESC dsdesc;
	ZeroMemory( &dsdesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	dsdesc.DepthEnable = true;
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsdesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	m_Device->CreateDepthStencilState(&dsdesc, &m_DepthState);

	D3D11_RASTERIZER_DESC rdesc;
	ZeroMemory( &rdesc, sizeof( D3D11_RASTERIZER_DESC ) );
	rdesc.FillMode = D3D11_FILL_SOLID;
	rdesc.CullMode = D3D11_CULL_BACK;
	//rdesc.FrontCounterClockwise = TRUE;
	m_Device->CreateRasterizerState(&rdesc,&m_RasterState);
}

void DeferredRenderer::renderLight(Shader *p_Shader, Buffer* p_ModelBuffer, vector<Light> *p_Lights)
{
	if(p_Lights->size() > 0)
	{
		UINT Offsets[2] = {0,0};
		ID3D11Buffer * buffers[] = {p_ModelBuffer->getBufferPointer(), m_AllLightBuffer->getBufferPointer()};
		UINT Stride[2] = {sizeof(DirectX::XMFLOAT3), sizeof(Light)};

		p_Shader->setShader();

		m_DeviceContext->UpdateSubresource(m_AllLightBuffer->getBufferPointer(),NULL,NULL,p_Lights->data(),NULL,NULL);

		m_DeviceContext->IASetVertexBuffers(0,2,buffers,Stride, Offsets);
		m_DeviceContext->DrawInstanced(p_ModelBuffer->getNumOfElements(), p_Lights->size(),0,0);
		m_DeviceContext->IASetVertexBuffers(0,0,0,0, 0);
		p_Shader->unSetShader();
	}
}
