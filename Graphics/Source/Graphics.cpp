#include "Graphics.h"
#include "ModelLoader.h"
#include <iostream>

const std::string Graphics::m_RelativeResourcePath = "../../Graphics/Resources/";

Graphics::Graphics(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_SwapChain = nullptr;
	m_RenderTargetView = nullptr;
	m_RasterState = nullptr;
	m_DepthStencilBuffer = nullptr;
	m_DepthStencilState = nullptr;
	m_DepthStencilView = nullptr;
	m_WrapperFactory = nullptr;
	m_DeferredRender = nullptr;

	m_VSyncEnabled = false; //DEBUG

	m_NextInstanceId = 1;
}

Graphics::~Graphics(void)
{
}

IGraphics *IGraphics::createGraphics()
{
	return new Graphics();
}

bool Graphics::initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight, bool p_Fullscreen)
{	
	HRESULT result;
	IDXGIFactory *factory;
	IDXGIAdapter *adapter;
	IDXGIOutput *adapterOutput;
	
	unsigned int numModes;
	unsigned int stringLength;
	
	DXGI_MODE_DESC *displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		throw GraphicsException("Error when creating DirectX graphics interface factory", __LINE__,__FILE__);
	}

	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		throw GraphicsException("Error when creating an adapter for the primary graphics interface", __LINE__,__FILE__);
	}

	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		throw GraphicsException("Error when enumerating the primary adapter output", __LINE__,__FILE__);
	}

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, NULL);
	if(FAILED(result))
	{
		throw GraphicsException("Error when getting the display modes", __LINE__,__FILE__);
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		throw GraphicsException("Error when creating display mode list", __LINE__,__FILE__);
	}

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, displayModeList);
	if(FAILED(result))
	{
		throw GraphicsException("Error when writing the display mode list", __LINE__,__FILE__);
	}

	for(unsigned int i = 0; i < numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)p_ScreenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)p_ScreenHeight)
			{
				m_Numerator = displayModeList[i].RefreshRate.Numerator;
				m_Denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		throw GraphicsException("Error when getting the graphics card description", __LINE__,__FILE__);
	}
	
	m_GraphicsMemory = (int)(adapterDesc.DedicatedVideoMemory / MB);

	error = wcstombs_s(&stringLength, m_GraphicsCard, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		throw GraphicsException("Error when storing the graphics card name", __LINE__,__FILE__);
	}

	SAFE_DELETE_ARRAY(displayModeList);
	SAFE_RELEASE(adapterOutput);
	SAFE_RELEASE(adapter);
	SAFE_RELEASE(factory);

	result = createDeviceAndSwapChain(p_Hwnd, p_ScreenWidth, p_ScreenHeight, p_Fullscreen);
	if(FAILED(result))
	{
		throw GraphicsException("Error when creating the device and swap chain", __LINE__,__FILE__);
	}

	result = createRenderTargetView();
	if(FAILED(result))
	{
		throw GraphicsException("Error when creating render target view", __LINE__,__FILE__);
	}

	result = createDepthStencilBuffer(p_ScreenWidth, p_ScreenHeight);
	if(FAILED(result))
	{
		throw GraphicsException("Error when creating the depth stencil buffer", __LINE__,__FILE__);
	}

	result = createDepthStencilState();
	if(FAILED(result))
	{
		throw GraphicsException("Error when creating the depth stencil state", __LINE__,__FILE__);
	}

	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

	result = createDepthStencilView();
	if(FAILED(result))
	{
		throw GraphicsException("Error when creating the depth stencil view", __LINE__,__FILE__);
	}

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	result = createRasterizerState();
	if(FAILED(result))
	{
		throw GraphicsException("Error when creating the rasterizer state", __LINE__,__FILE__);
	}

	m_DeviceContext->RSSetState(m_RasterState);

	setViewPort(p_ScreenWidth, p_ScreenHeight);

	//Note this is the only time initialize should be called.
	WrapperFactory::initialize(m_Device, m_DeviceContext);
	m_WrapperFactory = WrapperFactory::getInstance();

	m_VRAMMemInfo = VRAMMemInfo::getInstance();

	m_TextureLoader = TextureLoader(m_Device, m_DeviceContext);

	//Setup camera matrices REMOVE LATER
	DirectX::XMFLOAT4 eye4,lookat,up;
	DirectX::XMFLOAT3 eye;
	eye4 = DirectX::XMFLOAT4(0,0,-20,1);
	eye = DirectX::XMFLOAT3(eye4.x,eye4.y,eye4.z);
	lookat = DirectX::XMFLOAT4(0,0,0,1);
	up = DirectX::XMFLOAT4(0,1,0,0);
	DirectX::XMFLOAT4X4 view, proj;
	view = DirectX::XMFLOAT4X4();
	proj = DirectX::XMFLOAT4X4();
	DirectX::XMStoreFloat4x4(&view,
							DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(
								DirectX::XMLoadFloat4(&eye4),
								DirectX::XMLoadFloat4(&lookat),
								DirectX::XMLoadFloat4(&up))));
	DirectX::XMStoreFloat4x4(&proj,
							DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(
								0.25f*3.14f,
								(float)p_ScreenWidth / (float)p_ScreenHeight,
								0.1f,
								1000.0f)));

	//Deferred Render
	m_DeferredRender = new DeferredRenderer();
	m_DeferredRender->initialize(m_Device,m_DeviceContext, m_DepthStencilView,p_ScreenWidth, p_ScreenHeight,
								eye, view, proj);

	m_Shader = nullptr;
	createShader("DebugShader",L"../../Graphics/Source/DeferredShaders/DebugShader.hlsl","VS,PS","5_0",ShaderType::VERTEX_SHADER | IGraphics::ShaderType::PIXEL_SHADER);
	//m_WrapperFactory->addShaderStep(m_Shader,L,"VS","5_0",Shader::Type::VERTEX_SHADER);
	//m_WrapperFactory->addShaderStep(m_Shader,L"../../Graphics/Source/DeferredShaders/DebugShader.hlsl","PS","5_0",Shader::Type::PIXEL_SHADER);
	m_Shader = getShaderFromList("DebugShader");
	D3D11_SAMPLER_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.Filter                                = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU                                = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV                                = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW                                = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.ComparisonFunc                = D3D11_COMPARISON_NEVER;
    sd.MinLOD                                = 0;
    sd.MaxLOD                                = D3D11_FLOAT32_MAX;

    m_Sampler = nullptr;
    m_Device->CreateSamplerState( &sd, &m_Sampler );

	return true;
}

bool Graphics::reInitialize(HWND p_Hwnd, int p_ScreenWidht, int p_ScreenHeight, bool p_Fullscreen)
{
	HRESULT result = createDeviceAndSwapChain(p_Hwnd, p_ScreenWidht, p_ScreenHeight, p_Fullscreen);
	if(FAILED(result))
	{
		throw GraphicsException("Error when recreating the device and swap chain", __LINE__, __FILE__);
	}

	setViewPort(p_ScreenWidht, p_ScreenHeight);

	return true;
}

void Graphics::shutdown(void)
{
	if(m_SwapChain)
	{
		m_SwapChain->SetFullscreenState(false, NULL);
	}

	for(auto &s : m_ShaderList)
	{
		SAFE_DELETE(s.second);
	}
	//m_ShaderList.clear();
	SAFE_RELEASE(m_RasterState);
	SAFE_RELEASE(m_DepthStencilView);
	SAFE_RELEASE(m_DepthStencilState);
	SAFE_RELEASE(m_DepthStencilBuffer);
	SAFE_RELEASE(m_RenderTargetView);
	SAFE_RELEASE(m_DeviceContext);
	SAFE_RELEASE(m_Device);
	SAFE_RELEASE(m_SwapChain);
	m_WrapperFactory->shutdown();
	m_WrapperFactory = nullptr;
	m_VRAMMemInfo->shutdown();
	m_VRAMMemInfo = nullptr;
	
	//Deferred render
	SAFE_DELETE(m_DeferredRender);

	m_Shader = nullptr;
}

void IGraphics::deleteGraphics(IGraphics *p_Graphics)
{
	p_Graphics->shutdown();
	delete p_Graphics;
}

void Graphics::createModel(const char *p_ModelId, const char *p_Filename)
{
	ModelLoader modelLoader;
	Buffer *vertexBuffer = nullptr;

	modelLoader.loadFile(p_Filename);

	//int nrVertices = modelLoader.getVertices()->size();
	vector<std::vector<ModelLoader::IndexDesc>>	tempF	= modelLoader.getIndices();
	vector<DirectX::XMFLOAT3>				tempN	= modelLoader.getNormals();
	vector<DirectX::XMFLOAT3>				tempT	= modelLoader.getTangents();
	vector<DirectX::XMFLOAT2>				tempUV = modelLoader.getTextureCoords();
	vector<DirectX::XMFLOAT3>				tempVert = modelLoader.getVertices();
	vector<ModelLoader::Material>			tempM	= modelLoader.getMaterial();
	

	vector<vertex> temp;
	vector<vector<int>> tempI;

	vector<int> I;

	for(unsigned int i = 0; i < tempF.size(); i++)
	{
		const vector<ModelLoader::IndexDesc>& indexDescList = tempF.at(i);

		I.reserve(indexDescList.size());

		for(unsigned int j = 0; j < indexDescList.size();j++)
		{
			const ModelLoader::IndexDesc& indexDesc = indexDescList.at(j);

			temp.push_back(vertex(tempVert.at(indexDesc.m_Vertex),
									tempN.at(indexDesc.m_Normal),
									tempUV.at(indexDesc.m_TextureCoord),
									tempT.at(indexDesc.m_Tangent)));
			I.push_back(j);
		}

		tempI.push_back(I);
		I.clear();
	}
	
	// Create Vertex buffer.
	Buffer::Description bufferDescription;
	bufferDescription.initData = temp.data();
	bufferDescription.numOfElements = temp.size();
	bufferDescription.sizeOfElement = sizeof(Graphics::vertex);
	bufferDescription.type = Buffer::Type::VERTEX_BUFFER;
	bufferDescription.usage = Buffer::Usage::USAGE_IMMUTABLE; // Change to default when needed to change data.
	vertexBuffer = m_WrapperFactory->createBuffer(bufferDescription);
	temp.clear();

	// Create Index buffer.
	unsigned int nrIndexBuffers = tempI.size();
	Buffer **index = new Buffer*[nrIndexBuffers];
	bufferDescription.type = Buffer::Type::INDEX_BUFFER;
	//bufferDescription.usage = Buffer::Usage::USAGE_IMMUTABLE;// Change to default when needed to change data.
	bufferDescription.sizeOfElement = sizeof(int);
	
	//buffer = createBuffer(bufferDescription);
	
	for(unsigned int i = 0; i < nrIndexBuffers; i++)
	{
		bufferDescription.initData = tempI.at(i).data();
		bufferDescription.numOfElements = tempI.at(i).size();

		index[i] = WrapperFactory::getInstance()->createBuffer(bufferDescription);
	}
	tempI.clear();
	I.clear();
	
	// Load textures.
	ID3D11ShaderResourceView **diffuse	= new ID3D11ShaderResourceView*[nrIndexBuffers];
	ID3D11ShaderResourceView **normal	= new ID3D11ShaderResourceView*[nrIndexBuffers];
	ID3D11ShaderResourceView **specular = new ID3D11ShaderResourceView*[nrIndexBuffers];
	for(unsigned int i = 0; i < nrIndexBuffers; i++)
	{
		diffuse[i]	= m_TextureLoader.createTextureFromFile((m_RelativeResourcePath + tempM.at(i).m_DiffuseMap).c_str());
		normal[i]	= m_TextureLoader.createTextureFromFile((m_RelativeResourcePath + tempM.at(i).m_NormalMap).c_str());
		specular[i] = m_TextureLoader.createTextureFromFile((m_RelativeResourcePath + tempM.at(i).m_SpecularMap).c_str());

		ID3D11Resource *resource;
		ID3D11Texture2D *texture;
		D3D11_TEXTURE2D_DESC textureDesc;
		int size;

		diffuse[i]->GetResource(&resource);
		resource->QueryInterface(&texture);
		texture->GetDesc(&textureDesc);
		size = 4 * textureDesc.Width * textureDesc.Height;

		diffuse[i]->GetResource(&resource);
		resource->QueryInterface(&texture);
		texture->GetDesc(&textureDesc);
		size += 4 * textureDesc.Width * textureDesc.Height;
		
		diffuse[i]->GetResource(&resource);
		resource->QueryInterface(&texture);
		texture->GetDesc(&textureDesc);
		size += 4 * textureDesc.Width * textureDesc.Height;

		m_VRAMMemInfo->updateUsage(size);
	}
	Model m;
	m.vertexBuffer		= vertexBuffer;
	m.indexBuffer		= index;
	m.diffuseTexture	= diffuse;
	m.normalTexture		= normal;
	m.specularTexture	= specular;
	m.numOfMaterials	= nrIndexBuffers;

	m_ModelList.push_back(std::pair<string,Model>(p_ModelId,m));

	modelLoader.clear();
}

void Graphics::createShader(const char *p_shaderId, LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, ShaderType p_Type)
{
	bool found = false;
	Shader *shader;
	string entryPoint;
	vector<string> entryPointList;

	for(auto &s : m_ShaderList)
	{
		if(strcmp(s.first.c_str(), p_shaderId) == 0)
		{
			found = true;
			shader = s.second;
		}
	}
	if(!found)
	{
		shader = new Shader();
		shader->initialize(m_Device, m_DeviceContext, 0);
	}
	
	entryPointList = createEntryPointList(p_EntryPoint);

	try
	{
		if((p_Type & ShaderType::VERTEX_SHADER))
		{
			entryPoint = entryPointList.back();
			entryPointList.pop_back();
			m_WrapperFactory->addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
				Shader::Type::VERTEX_SHADER);
		}
		if((p_Type & ShaderType::PIXEL_SHADER))
		{
			entryPoint = entryPointList.back();
			entryPointList.pop_back();
			m_WrapperFactory->addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
				Shader::Type::PIXEL_SHADER);
		}
		if((p_Type & ShaderType::GEOMETRY_SHADER))
		{
			entryPoint = entryPointList.back();
			entryPointList.pop_back();
			m_WrapperFactory->addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
				Shader::Type::GEOMETRY_SHADER);
		}
		if((p_Type & ShaderType::HULL_SHADER))
		{
			entryPoint = entryPointList.back();
			entryPointList.pop_back();
			m_WrapperFactory->addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
				Shader::Type::HULL_SHADER);
		}
		if((p_Type & ShaderType::DOMAIN_SHADER))
		{
			entryPoint = entryPointList.back();
			entryPointList.pop_back();
			m_WrapperFactory->addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
				Shader::Type::DOMAIN_SHADER);
		}
	}
	catch(...)
	{
		if(!found)
		{
			SAFE_DELETE(shader);
		}

		
		throw;
	}

	if(!found)
	{
		m_ShaderList.push_back(make_pair(p_shaderId, shader));
	}
}

void Graphics::createShader(const char *p_shaderId, LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, ShaderType p_Type, ShaderInputElementDescription *p_VertexLayout,
	unsigned int p_NumOfElements)

{
	if(!(p_Type & ShaderType::VERTEX_SHADER))
		throw ShaderException("Failed to create shader, no vertex shader defined", __LINE__, __FILE__);

	for(auto &s : m_ShaderList)
	{
		if(strcmp(s.first.c_str(), p_shaderId) == 0)
		{
			throw ShaderException("Failed to create shader, shader already exists", __LINE__, __FILE__);
		}
	}

	vector<string> entryPointList;
	string entryPoint;

	Shader *shader = new Shader();
	shader->initialize(m_Device, m_DeviceContext, p_NumOfElements);

	D3D11_INPUT_ELEMENT_DESC *desc = new D3D11_INPUT_ELEMENT_DESC[p_NumOfElements];
	for(unsigned int i = 0; i < p_NumOfElements; i++)
	{
		desc[i].SemanticName = p_VertexLayout[i].semanticName;
		desc[i].SemanticIndex = p_VertexLayout[i].semanticIndex; 
		desc[i].Format = (DXGI_FORMAT)p_VertexLayout[i].format;
		desc[i].InputSlot = p_VertexLayout[i].inputSlot;
		desc[i].AlignedByteOffset = p_VertexLayout[i].alignedByteOffset;
		desc[i].InputSlotClass = (D3D11_INPUT_CLASSIFICATION)p_VertexLayout[i].inputSlotClass;
		desc[i].InstanceDataStepRate = p_VertexLayout[i].instanceDataStepRate;
	}
	
	entryPointList = createEntryPointList(p_EntryPoint);

	try
	{
		if((p_Type & ShaderType::VERTEX_SHADER))
		{
			entryPoint = entryPointList.back();
			entryPointList.pop_back();
			m_WrapperFactory->addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
				Shader::Type::VERTEX_SHADER, desc);

			SAFE_DELETE(desc);
			desc = nullptr;
		}
		if((p_Type & ShaderType::PIXEL_SHADER))
		{
			entryPoint = entryPointList.back();
			entryPointList.pop_back();
			m_WrapperFactory->addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
				Shader::Type::PIXEL_SHADER);
		}
		if((p_Type & ShaderType::GEOMETRY_SHADER))
		{
			entryPoint = entryPointList.back();
			entryPointList.pop_back();
			m_WrapperFactory->addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
				Shader::Type::GEOMETRY_SHADER);
		}
		if((p_Type & ShaderType::HULL_SHADER))
		{
			entryPoint = entryPointList.back();
			entryPointList.pop_back();
			m_WrapperFactory->addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
				Shader::Type::HULL_SHADER);
		}
		if((p_Type & ShaderType::DOMAIN_SHADER))
		{
			entryPoint = entryPointList.back();
			entryPointList.pop_back();
			m_WrapperFactory->addShaderStep(shader, p_Filename, entryPoint.c_str(), p_ShaderModel,
				Shader::Type::DOMAIN_SHADER);
		}

		m_ShaderList.push_back(make_pair(p_shaderId, shader));
	}
	catch(...)
	{
		SAFE_DELETE(shader);
		SAFE_DELETE(desc);
		throw;
	}
}

void Graphics::linkShaderToModel(const char *p_ShaderId, const char *p_ModelId)
{
	Model* m = nullptr;
	m = getModelFromList(p_ModelId);
	if(m != nullptr)
		m->shader = getShaderFromList(p_ShaderId);
	m = nullptr;
}

void Graphics::createTexture(const char *p_TextureId, const char *p_Filename)
{
	m_TextureList.push_back(make_pair(p_TextureId, m_TextureLoader.createTextureFromFile(p_Filename)));
	
	unsigned int textureSize = sizeof(m_TextureList.back().second);
	
	m_VRAMMemInfo->updateUsage(textureSize);
}

void Graphics::renderModel(int p_ModelId)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_ModelId)
		{
			m_DeferredRender->addRenderable(DeferredRenderer::Renderable(getModelFromList(inst.second.m_ModelName), &inst.second.getWorldMatrix()));
			break;
		}
	}
}

void Graphics::renderText(void)
{
	
}

void Graphics::renderQuad(void)
{

}

void Graphics::addStaticLight(void)
{

}

void Graphics::removeStaticLight(void)
{

}

void Graphics::useFrameLight(void)
{

}

void Graphics::drawFrame(int i)
{
	float color[4] = {0.0f, 0.5f, 0.0f, 1.0f}; 
	Begin(color);

	m_DeferredRender->renderDeferred();

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView); 
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_Shader->setShader();
	m_Shader->setResource(Shader::Type::PIXEL_SHADER,0,1,m_DeferredRender->getRT(i));
	m_Shader->setSamplerState(Shader::Type::PIXEL_SHADER, 0, 1, m_Sampler);
	m_DeviceContext->Draw(6,0);

	m_Shader->unSetShader();
	
	End();
}

int Graphics::getVRAMMemUsage(void)
{
	return m_VRAMMemInfo->getUsage();
}

int Graphics::createModelInstance(const char *p_ModelId)
{
	ModelInstance instance;
	instance.m_IsCalculated = false;
	instance.m_ModelName = p_ModelId;
	instance.m_Position = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	instance.m_Rotation = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	instance.m_Scale = DirectX::XMFLOAT3(1.f, 1.f, 1.f);

	int id = m_NextInstanceId++;
	m_ModelInstances.push_back(std::make_pair(id, instance));

	return id;
}

void Graphics::eraseModelInstance(int p_Instance)
{
	for (unsigned int i = 0; i < m_ModelInstances.size(); i++)
	{
		if (m_ModelInstances.at(i).first == p_Instance)
		{
			m_ModelInstances.erase(m_ModelInstances.begin() + i);
			return;
		}
	}
}

void Graphics::setModelPosition(int p_Instance, float p_X, float p_Y, float p_Z)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.setPosition(DirectX::XMFLOAT3(p_X, p_Y, p_Z));
			break;
		}
	}
}

void Graphics::setModelRotation(int p_Instance, float p_Yaw, float p_Pitch, float p_Roll)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.setRotation(DirectX::XMFLOAT3(p_Pitch, p_Yaw, p_Roll));
			break;
		}
	}
}

void Graphics::setModelScale(int p_Instance, float p_X, float p_Y, float p_Z)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.setScale(DirectX::XMFLOAT3(p_X, p_Y, p_Z));
			break;
		}
	}
}

void Graphics::updateCamera(float p_PosX, float p_PosY, float p_PosZ, float p_Yaw, float p_Pitch)
{
	using DirectX::operator+;

	DirectX::XMFLOAT4 eye(p_PosX, p_PosY, p_PosZ, 1.f);
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&eye);

	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(p_Pitch, p_Yaw, 0.f);

	static const DirectX::XMFLOAT4 up(0.f, 1.f, 0.f, 0.f);
	DirectX::XMVECTOR upVec = DirectX::XMLoadFloat4(&up);

	DirectX::XMVECTOR rotatedUp = DirectX::XMVector4Transform(upVec, rotation);

	static const DirectX::XMFLOAT4 forward(0.f, 0.f, -1.f, 0.f);
	DirectX::XMVECTOR forwardVec = DirectX::XMLoadFloat4(&forward);

	DirectX::XMVECTOR lookAt = pos + DirectX::XMVector4Transform(forwardVec, rotation);

	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view,
							DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(pos, lookAt, rotatedUp)));

	m_DeferredRender->updateViewMatrix(view);
	m_DeferredRender->updateCameraPosition(DirectX::XMFLOAT3(p_PosX, p_PosY, p_PosZ));
}

void Graphics::setViewPort(int p_ScreenWidth, int p_ScreenHeight)
{
	D3D11_VIEWPORT viewport;

	viewport.Width = (float)p_ScreenWidth;
	viewport.Height = (float)p_ScreenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_DeviceContext->RSSetViewports(1, &viewport);
}

HRESULT Graphics::createDeviceAndSwapChain(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight,
	bool p_Fullscreen)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;

	//Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = p_ScreenWidth;
	swapChainDesc.BufferDesc.Height = p_ScreenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if(m_VSyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = m_Numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = m_Denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = p_Hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if(p_Fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	return D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
		D3D11_SDK_VERSION, &swapChainDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext);
}

HRESULT Graphics::createRenderTargetView(void)
{
	HRESULT result;
	ID3D11Texture2D *backBufferPtr;

	//Get the pointer to the back buffer.
	result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(result))
	{
		SAFE_RELEASE(backBufferPtr);

		return result;
	}

	//Create the render target view with the back buffer pointer.
	m_Device->CreateRenderTargetView(backBufferPtr, NULL, &m_RenderTargetView);
	SAFE_RELEASE(backBufferPtr);

	return result;
}

HRESULT Graphics::createDepthStencilBuffer(int p_ScreenWidth, int p_ScreenHeight)
{
	D3D11_TEXTURE2D_DESC depthBufferDesc;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));


	//Set up the description of the depth buffer.
	depthBufferDesc.Width = p_ScreenWidth;
	depthBufferDesc.Height = p_ScreenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;//DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//Create the texture for the depth buffer using the filled out description.
	return m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_DepthStencilBuffer);
}

HRESULT Graphics::createDepthStencilState(void)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	//Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	return m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);
}

HRESULT Graphics::createDepthStencilView(void)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	//Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	return m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView);
}

HRESULT Graphics::createRasterizerState(void)
{
	D3D11_RASTERIZER_DESC rasterDesc;

	//Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	return m_Device->CreateRasterizerState(&rasterDesc, &m_RasterState);
}

Buffer *Graphics::createBuffer(Buffer::Description &p_Description)
{
	return m_WrapperFactory->createBuffer(p_Description);
}

vector<string> Graphics::createEntryPointList(const char *p_EntryPoint)
{
	vector<string> entryList;
	vector<string> result;

	std::vector<char> buffer(strlen(p_EntryPoint)+1);
	strcpy(buffer.data(), p_EntryPoint);
	char *tmp;
	tmp = strtok(buffer.data(), ",");
	while(tmp != nullptr)
	{
		entryList.push_back(tmp);
		tmp = strtok(NULL,",");
	}

	for(int i = entryList.size() - 1; i >= 0; i--)
	{
		result.push_back(entryList.at(i));
	}

	return result;
}

void Graphics::Begin(float color[4])
{
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, color);

	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


void Graphics::End(void)
{
	if(m_VSyncEnabled)
	{
		// Lock to screen refresh rate.
		m_SwapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_SwapChain->Present(0, 0);
	}
}

Shader* Graphics::getShaderFromList(string p_Identifier)
{
	Shader* ret = nullptr;

	for(auto & s : m_ShaderList)
	{
		if(s.first.compare(p_Identifier) == 0 )
		{
			ret = s.second;
			break;
		}
	}
	return ret;
}

Model* Graphics::getModelFromList(string p_Identifier)
{
	Model* ret = nullptr;

	for(auto & s : m_ModelList)
	{
		if(s.first.compare(p_Identifier) == 0 )
		{
			ret = &s.second;
			break;
		}
	}
	return ret;
}