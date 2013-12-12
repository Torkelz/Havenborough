#include "Graphics.h"
#include "GraphicsLogger.h"

#include <iostream>
#include <boost/filesystem.hpp>

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
	m_ModelFactory = nullptr;
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
	GraphicsLogger::log(GraphicsLogger::Level::INFO, "Initializing graphics");

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
	m_ModelFactory = ModelFactory::getInstance();
	m_ModelFactory->initialize(&m_TextureList);

	m_TextureLoader = TextureLoader(m_Device, m_DeviceContext);

	initializeMatrices(p_ScreenWidth, p_ScreenHeight);

	//Deferred Render
	m_DeferredRender = new DeferredRenderer();
	m_DeferredRender->initialize(m_Device,m_DeviceContext, m_DepthStencilView,p_ScreenWidth, p_ScreenHeight,
		&m_Eye, &m_ViewMatrix, &m_ProjectionMatrix);
	
	DebugDefferedDraw();

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
	GraphicsLogger::log(GraphicsLogger::Level::INFO, "Shutting down graphics");

	if(m_SwapChain)
	{
		m_SwapChain->SetFullscreenState(false, NULL);
	}

	for(auto &s : m_ShaderList)
	{
		SAFE_DELETE(s.second);
	}
	m_ShaderList.clear();

	for (auto& tex : m_TextureList)
	{
		SAFE_RELEASE(tex.second);
	}
	m_TextureList.clear();
	m_ModelList.clear();

	SAFE_RELEASE(m_Sampler);
	SAFE_RELEASE(m_RasterState);
	SAFE_RELEASE(m_DepthStencilView);
	SAFE_RELEASE(m_DepthStencilState);
	SAFE_RELEASE(m_DepthStencilBuffer);
	SAFE_RELEASE(m_RenderTargetView);
	SAFE_RELEASE(m_DeviceContext);
	SAFE_RELEASE(m_Device);
	SAFE_RELEASE(m_SwapChain);
	SAFE_SHUTDOWN(m_WrapperFactory);
	SAFE_SHUTDOWN(m_ModelFactory);
	SAFE_SHUTDOWN(m_VRAMMemInfo);

	//Deferred render
	SAFE_DELETE(m_DeferredRender);

	m_Shader = nullptr;
}

void IGraphics::deleteGraphics(IGraphics *p_Graphics)
{
	p_Graphics->shutdown();
	delete p_Graphics;
}

bool Graphics::createModel(const char *p_ModelId, const char *p_Filename)
{
	ModelDefinition model =	m_ModelFactory->getInstance()->createStaticModel(p_Filename);

	m_ModelList.push_back(std::pair<string,ModelDefinition>(p_ModelId, std::move(model)));

	return true;
}
	
bool Graphics::createAnimatedModel(const char *p_ModelId, const char *p_Filename)
{
	ModelDefinition model = m_ModelFactory->getInstance()->createAnimatedModel(p_Filename);
	return true;
}

bool Graphics::releaseModel(const char* p_ResourceName)
{
	for(auto it = m_ModelList.begin(); it != m_ModelList.end(); ++it)
	{
		if(strcmp(it->first.c_str(), p_ResourceName) == 0)
		{
			for(unsigned int i = 0; i < it->second.numOfMaterials; i++)
			{
				m_ReleaseModelTexture(it->second.diffuseTexture[i].first.c_str(), m_ReleaseModelTextureUserdata);
				m_ReleaseModelTexture(it->second.normalTexture[i].first.c_str(), m_ReleaseModelTextureUserdata);
				m_ReleaseModelTexture(it->second.specularTexture[i].first.c_str(), m_ReleaseModelTextureUserdata);
			}

			m_ModelList.erase(it);
			return true;
		}
	}
	return false;
}

void Graphics::createShader(const char *p_shaderId, LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, ShaderType p_Type)
{
	bool found = false;
	Shader *shader;

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
		shader = m_WrapperFactory->createShader(p_Filename, p_EntryPoint, p_ShaderModel, p_Type);
		m_ShaderList.push_back(make_pair(p_shaderId, shader));
	}
	else
	{
		m_WrapperFactory->addShaderStep(shader, p_Filename, p_EntryPoint, p_ShaderModel, p_Type);
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

	m_ShaderList.push_back(make_pair(p_shaderId, m_WrapperFactory->createShader(p_Filename, p_EntryPoint,
		p_ShaderModel, p_Type, p_VertexLayout, p_NumOfElements)));
}

void Graphics::linkShaderToModel(const char *p_ShaderId, const char *p_ModelId)
{
	ModelDefinition *model = nullptr;
	model = getModelFromList(p_ModelId);
	if(model != nullptr)
		model->shader = getShaderFromList(p_ShaderId);
	model = nullptr;
}

bool Graphics::createTexture(const char *p_TextureId, const char *p_Filename)
{
	ID3D11ShaderResourceView *resourceView = m_TextureLoader.createTextureFromFile(p_Filename);
	if(!resourceView)
	{
		return false;
	}

	int size = calculateTextureSize(resourceView);
	m_VRAMMemInfo->updateUsage(size);

	m_TextureList.push_back(make_pair(p_TextureId, resourceView));

	return true;
}

bool Graphics::releaseTexture(const char *p_TextureId)
{
	for(auto it = m_TextureList.begin(); it != m_TextureList.end(); ++it)
	{
		if(strcmp(it->first.c_str(), p_TextureId) == 0)
		{
			ID3D11ShaderResourceView *&m = it->second;
			int size = calculateTextureSize(m);
			m_VRAMMemInfo->updateUsage(-size);

			SAFE_RELEASE(m);
			m_TextureList.erase(it);
			return true;
		}
	}

	return false;
}

void Graphics::renderModel(int p_ModelId)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_ModelId)
		{
			m_DeferredRender->addRenderable(DeferredRenderer::Renderable(getModelFromList(inst.second.m_ModelName),
				&inst.second.getWorldMatrix()));
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
	using namespace DirectX;

	m_Eye = XMFLOAT3(p_PosX,p_PosY,p_PosZ);
	XMFLOAT4 eye(m_Eye.x, m_Eye.y, m_Eye.z, 1.f);
	XMVECTOR pos = XMLoadFloat4(&eye);

	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(p_Pitch, p_Yaw, 0.f);

	static const XMFLOAT4 up(0.f, 1.f, 0.f, 0.f);
	XMVECTOR upVec = XMLoadFloat4(&up);

	XMVECTOR rotatedUp = XMVector4Transform(upVec, rotation);

	static const XMFLOAT4 forward(0.f, 0.f, -1.f, 0.f);
	XMVECTOR forwardVec = XMLoadFloat4(&forward);

	XMVECTOR lookAt = pos + XMVector4Transform(forwardVec, rotation);

	//XMFLOAT4X4 view;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(pos, lookAt, rotatedUp)));
}

void Graphics::setLogFunction(clientLogCallback_t p_LogCallback)
{
	GraphicsLogger::setLogFunction(p_LogCallback);
}

void Graphics::setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void* p_Userdata)
{
	m_ModelFactory->setLoadModelTextureCallBack(p_LoadModelTexture, p_Userdata);
}

void Graphics::setReleaseModelTextureCallBack(releaseModelTextureCallBack p_ReleaseModelTexture, void *p_Userdata)
{
	m_ReleaseModelTexture = p_ReleaseModelTexture;
	m_ReleaseModelTextureUserdata = p_Userdata;
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

	//Initiailize the depth stencil view.
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
	rasterDesc.CullMode = D3D11_CULL_FRONT;
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

void Graphics::initializeMatrices( int p_ScreenWidth, int p_ScreenHeight )
{
	XMFLOAT4 eye;
	XMFLOAT4 lookAt;
	XMFLOAT4 up;
	m_Eye = XMFLOAT3(0,0,-20);

	eye = XMFLOAT4(m_Eye.x,m_Eye.y,m_Eye.z,1);
	lookAt = XMFLOAT4(0,0,0,1);
	up = XMFLOAT4(0,1,0,0);
	m_ViewMatrix = XMFLOAT4X4();
	m_ProjectionMatrix = XMFLOAT4X4();

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&eye),
		XMLoadFloat4(&lookAt), XMLoadFloat4(&up))));
	XMStoreFloat4x4(&m_ProjectionMatrix, XMMatrixTranspose(XMMatrixPerspectiveFovLH(0.25f * 3.14f,
		(float)p_ScreenWidth / (float)p_ScreenHeight, 0.1f, 1000.0f)));
}

Shader *Graphics::getShaderFromList(string p_Identifier)
{
	Shader *ret = nullptr;

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

ModelDefinition *Graphics::getModelFromList(string p_Identifier)
{
	ModelDefinition* ret = nullptr;

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

ID3D11ShaderResourceView *Graphics::getTextureFromList(string p_Identifier)
{
	for(auto & s : m_TextureList)
	{
		if(s.first == p_Identifier)
		{
			return s.second;
		}
	}

	return nullptr;
}

int Graphics::calculateTextureSize(ID3D11ShaderResourceView *resourceView )
{
	ID3D11Resource *resource;
	ID3D11Texture2D *texture;
	D3D11_TEXTURE2D_DESC textureDesc;

	resourceView->GetResource(&resource);
	resource->QueryInterface(&texture);
	texture->GetDesc(&textureDesc);

	SAFE_RELEASE(texture);
	SAFE_RELEASE(resource);

	return m_VRAMMemInfo->calculateFormatUsage(textureDesc.Format, textureDesc.Width, textureDesc.Height);
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

//TODO: Remove later
void Graphics::DebugDefferedDraw(void)
{
	m_Shader = nullptr;
	createShader("DebugShader",L"../../Graphics/Source/DeferredShaders/DebugShader.hlsl","VS,PS","5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	//m_WrapperFactory->addShaderStep(m_Shader,L,"VS","5_0",Shader::Type::VERTEX_SHADER);
	//m_WrapperFactory->addShaderStep(m_Shader,L"../../Graphics/Source/DeferredShaders/DebugShader.hlsl","PS","5_0",Shader::Type::PIXEL_SHADER);
	m_Shader = getShaderFromList("DebugShader");
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter		= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU     = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW		 = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc                = D3D11_COMPARISON_NEVER;
	sd.MinLOD                       = 0;
	sd.MaxLOD                        = D3D11_FLOAT32_MAX;

	m_Sampler = nullptr;
	m_Device->CreateSamplerState( &sd, &m_Sampler );
}