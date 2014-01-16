#include "Graphics.h"
#include "GraphicsLogger.h"

#include <iostream>
#include <boost/filesystem.hpp>

using namespace DirectX;

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
	m_Sampler = nullptr;
	m_VRAMMemInfo = nullptr;

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
	IDXGIFactory *factory = nullptr;
	IDXGIAdapter *adapter = nullptr;
	IDXGIOutput *adapterOutput = nullptr;
	
	unsigned int numModes;
	unsigned int stringLength;
	
	DXGI_MODE_DESC *displayModeList = nullptr;
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
		&m_Eye, &m_ViewMatrix, &m_ProjectionMatrix, &m_SpotLights, &m_PointLights, &m_DirectionalLights);
	
	DebugDefferedDraw();
	setClearColor(Vector4(0.0f, 0.5f, 0.0f, 1.0f)); 
	m_BVBufferNumOfElements = 100;
	Buffer::Description buffDesc;
	buffDesc.initData = nullptr;
	buffDesc.numOfElements = m_BVBufferNumOfElements;
	buffDesc.sizeOfElement = sizeof(XMFLOAT4);
	buffDesc.type = Buffer::Type::VERTEX_BUFFER;
	buffDesc.usage = Buffer::Usage::DEFAULT;
	
	m_BVBuffer = WrapperFactory::getInstance()->createBuffer(buffDesc);

	VRAMMemInfo::getInstance()->updateUsage(sizeof(XMFLOAT4) * m_BVBufferNumOfElements);

	//ShaderInputElementDescription shaderDesc[] = 
	//{
	//	{"POSITION", 0, Format::R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	//};

	m_BVShader = WrapperFactory::getInstance()->createShader(L"../../Graphics/Source/DeferredShaders/BoundingVolume.hlsl",
															"VS,PS", "5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);//, shaderDesc, 1);
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

	while (!m_ModelList.empty())
	{
		std::string unremovedName = m_ModelList.front().first;

		GraphicsLogger::log(GraphicsLogger::Level::WARNING, "Model '" + unremovedName + "' not removed properly");

		releaseModel(unremovedName.c_str());
	}

	SAFE_RELEASE(m_Sampler);
	SAFE_RELEASE(m_RasterState);
	SAFE_RELEASE(m_RasterStateBV);
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

	SAFE_DELETE(m_BVBuffer);
	SAFE_DELETE(m_BVShader);

	//Deferred render
	SAFE_DELETE(m_DeferredRender);
	//Clear lights
	m_PointLights.clear();
	m_SpotLights.clear();
	m_DirectionalLights.clear();

	m_PointLights.shrink_to_fit();
	m_SpotLights.shrink_to_fit();
	m_DirectionalLights.shrink_to_fit();

	VRAMMemInfo::getInstance()->updateUsage(-(int)(sizeof(XMFLOAT4) * m_BVBufferNumOfElements));
	
	m_Shader = nullptr;
}

void IGraphics::deleteGraphics(IGraphics *p_Graphics)
{
	p_Graphics->shutdown();
	delete p_Graphics;
}

bool Graphics::createModel(const char *p_ModelId, const char *p_Filename)
{
	ModelDefinition model =	m_ModelFactory->getInstance()->createModel(p_Filename);

	m_ModelList.push_back(pair<string, ModelDefinition>(p_ModelId, std::move(model)));

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
	Shader *shader = nullptr;

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

void Graphics::linkShaderToModel(const char *p_ShaderId, const char *p_ModelId) //TODO: Maybe need to handle if animated or static?
{
	ModelDefinition *model = nullptr;
	model = getModelFromList(p_ModelId);
	if(model != nullptr)
		model->shader = getShaderFromList(p_ShaderId);
}

void Graphics::deleteShader(const char *p_ShaderId)
{
	for(auto & s : m_ShaderList)
	{
		if(s.first.compare(p_ShaderId) == 0 )
		{
			SAFE_DELETE(s.second);
			std::swap(s, m_ShaderList.back());
			m_ShaderList.pop_back();
			break;
		}
	}
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

void Graphics::renderModel(int p_ModelId) //TODO: Maybe need to handle if animated or static?
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_ModelId)
		{
			m_DeferredRender->addRenderable(DeferredRenderer::Renderable(getModelFromList(inst.second.getModelName()),
				inst.second.getWorldMatrix(),
				&inst.second.getFinalTransform()));
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

void Graphics::useFramePointLight(Vector3 p_LightPosition, Vector3 p_LightColor, float p_LightRange)
{
	Light l;
	l.lightPos = XMFLOAT3(p_LightPosition.x,p_LightPosition.y,p_LightPosition.z);
	l.lightColor = XMFLOAT3(p_LightColor.x,p_LightColor.y,p_LightColor.z);
	l.lightRange = p_LightRange;
	m_PointLights.push_back(l);
}
void Graphics::useFrameSpotLight(Vector3 p_LightPosition, Vector3 p_LightColor, Vector3 p_LightDirection,
	Vector2 p_SpotLightAngles,	float p_LightRange)
{
	Light l;
	l.lightPos = XMFLOAT3(p_LightPosition.x,p_LightPosition.y,p_LightPosition.z);
	l.lightColor = XMFLOAT3(p_LightColor.x,p_LightColor.y,p_LightColor.z);
	
	XMFLOAT3 lightDirection = Vector3ToXMFLOAT3(&p_LightDirection);
	XMVECTOR lightDirectionV = XMVector3Normalize(XMLoadFloat3(&lightDirection));

	XMStoreFloat3(&l.lightDirection, lightDirectionV);
	l.spotlightAngles = XMFLOAT2(p_SpotLightAngles.x,p_SpotLightAngles.y);
	l.lightRange = p_LightRange;
	m_SpotLights.push_back(l);
}
void Graphics::useFrameDirectionalLight(Vector3 p_LightColor, Vector3 p_LightDirection)
{
	Light l;
	l.lightColor = XMFLOAT3(p_LightColor.x,p_LightColor.y,p_LightColor.z);

	XMFLOAT3 lightDirection = Vector3ToXMFLOAT3(&p_LightDirection);
	XMVECTOR lightDirectionV = XMVector3Normalize(XMLoadFloat3(&lightDirection));

	XMStoreFloat3(&l.lightDirection, lightDirectionV);
	m_DirectionalLights.push_back(l);
}

void Graphics::setClearColor(Vector4 p_Color)
{
	m_ClearColor[0] = p_Color.x;
	m_ClearColor[1] = p_Color.y;
	m_ClearColor[2] = p_Color.z;
	m_ClearColor[3] = p_Color.w;
}

void Graphics::drawFrame(int i)
{
	if (!m_DeviceContext || !m_DeferredRender)
	{
		return;
	}

	Begin(m_ClearColor);

	m_DeferredRender->renderDeferred();

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, NULL); 
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if(i >= 0 && i <=3)
	{
		m_Shader->setShader();
		m_Shader->setResource(Shader::Type::PIXEL_SHADER, 0, 1, m_DeferredRender->getRT(i));
		m_Shader->setSamplerState(Shader::Type::PIXEL_SHADER, 0, 1, m_Sampler);
		m_DeviceContext->Draw(6, 0);

		m_Shader->unSetShader();
	}

	drawBoundingVolumes();

	End();

	//Delete lights
	m_PointLights.clear();
	m_SpotLights.clear();
	m_DirectionalLights.clear();
}

void Graphics::updateAnimations(float p_DeltaTime)
{
	for (auto& model : m_ModelInstances)
	{
		ModelDefinition* modelDef = getModelFromList(model.second.getModelName());
		if (modelDef->m_IsAnimated)
		{
			model.second.updateAnimation(p_DeltaTime, modelDef->m_Joints);
		}
	}
}

void Graphics::playAnimation(int p_Instance, char* p_ClipName)
{
	#include "AnimationStructs.h"

	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			const ModelDefinition* modelDef = getModelFromList(inst.second.getModelName());
			//ModelDefinition* modelDef = getModelFromList(inst.second.getModelName());
			std::string tempStr(p_ClipName);

			// If an illegal string has been put in, just shoot in the default animation.
			// The show must go on!
			if( modelDef->m_AnimationClips.find("default") != modelDef->m_AnimationClips.end() )
			{
				tempStr = "default";
			}

			inst.second.playClip(modelDef->m_AnimationClips.at(tempStr));
			break;
		}
	}
}

int Graphics::getVRAMMemUsage(void)
{
	if (m_VRAMMemInfo)
	{
		return m_VRAMMemInfo->getUsage();
	}
	else
	{
		return 0;
	}
}

int Graphics::createModelInstance(const char *p_ModelId)
{
	ModelDefinition* modelDef = getModelFromList(p_ModelId);
	if (modelDef == nullptr)
	{
		GraphicsLogger::log(GraphicsLogger::Level::ERROR_L, "Attempting to create model instance without loading the mode definition: " + std::string(p_ModelId));
		return -1;
	}

	ModelInstance instance;
	instance.setModelName(p_ModelId);
	instance.setPosition(XMFLOAT3(0.f, 0.f, 0.f));
	instance.setRotation(XMFLOAT3(0.f, 0.f, 0.f));
	instance.setScale(XMFLOAT3(1.f, 1.f, 1.f));
	int id = m_NextInstanceId++;

	if (modelDef->m_IsAnimated)
	{
		instance.updateAnimation(0.f, modelDef->m_Joints);
	}

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

void Graphics::setModelPosition(int p_Instance, Vector3 p_Position)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.setPosition(Vector3ToXMFLOAT3(&p_Position));
			break;
		}
	}
}

void Graphics::setModelRotation(int p_Instance, Vector3 p_YawPitchRoll)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.setRotation(DirectX::XMFLOAT3(p_YawPitchRoll.y, p_YawPitchRoll.x, p_YawPitchRoll.z));
			break;
		}
	}
}

void Graphics::setModelScale(int p_Instance, Vector3 p_Scale)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.setScale(DirectX::XMFLOAT3(p_Scale));
			break;
		}
	}
}

void Graphics::applyIK_ReachPoint(int p_Instance, const char* p_TargetJoint, const char* p_HingeJoint, const char* p_BaseJoint, Vector3 p_Target)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.applyIK_ReachPoint(p_TargetJoint, p_HingeJoint, p_BaseJoint, p_Target, getModelFromList(inst.second.getModelName())->m_Joints);
			break;
		}
	}
}

Vector3 Graphics::getJointPosition(int p_Instance, const char* p_Joint)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			const ModelDefinition* modelDef = getModelFromList(inst.second.getModelName());
			XMFLOAT3 position = inst.second.getJointPos(p_Joint, modelDef->m_Joints);
			
			return position;
		}
	}

	throw InvalidArgumentGraphicsException("Model instance does not exist", __LINE__, __FILE__);
}

void Graphics::updateCamera(Vector3 p_Position, float p_Yaw, float p_Pitch)
{
	using namespace DirectX;

	m_Eye = Vector3ToXMFLOAT3(&p_Position);
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

void Graphics::addBVTriangle(Vector3 p_Corner1, Vector3 p_Corner2, Vector3 p_Corner3)
{
	m_BVTriangles.push_back(Vector3ToXMFLOAT4(&p_Corner1, 1.f));
	m_BVTriangles.push_back(Vector3ToXMFLOAT4(&p_Corner2, 1.f));
	m_BVTriangles.push_back(Vector3ToXMFLOAT4(&p_Corner3, 1.f));
}

void Graphics::setLogFunction(clientLogCallback_t p_LogCallback)
{
	GraphicsLogger::setLogFunction(p_LogCallback);
}

void Graphics::setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void *p_Userdata)
{
	if (m_ModelFactory)
	{
		m_ModelFactory->setLoadModelTextureCallBack(p_LoadModelTexture, p_Userdata);
	}
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
	HRESULT result;

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

	result =  m_Device->CreateRasterizerState(&rasterDesc, &m_RasterState);
	
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	result =  m_Device->CreateRasterizerState(&rasterDesc, &m_RasterStateBV);
	return result;
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
		(float)p_ScreenWidth / (float)p_ScreenHeight, 10.f, 100000.0f)));
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
	for(auto & s : m_ModelList)
	{
		if(s.first == p_Identifier)
		{
			return &s.second;
		}
	}

	return nullptr;
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

void Graphics::drawBoundingVolumes()
{
	if(m_BVTriangles.size() > 0)
	{
		Buffer* buffer = nullptr;

		if(m_BVTriangles.size() >= m_BVBufferNumOfElements)
		{
			VRAMMemInfo::getInstance()->updateUsage(-(int)(sizeof(XMFLOAT4) * m_BVBufferNumOfElements));
			m_BVBufferNumOfElements = m_BVTriangles.size() + 1;
			Buffer::Description buffDesc;
			buffDesc.initData = &m_BVTriangles;
			buffDesc.numOfElements = m_BVBufferNumOfElements;
			buffDesc.sizeOfElement = sizeof(XMFLOAT4);
			buffDesc.type = Buffer::Type::VERTEX_BUFFER;
			buffDesc.usage = Buffer::Usage::DEFAULT;
	
			buffer = WrapperFactory::getInstance()->createBuffer(buffDesc);
			VRAMMemInfo::getInstance()->updateUsage(sizeof(XMFLOAT4) * m_BVBufferNumOfElements);
			SAFE_DELETE(m_BVBuffer);
			m_BVBuffer = buffer;
		}
		else 
		{
			m_DeviceContext->UpdateSubresource(m_BVBuffer->getBufferPointer(), NULL, NULL, m_BVTriangles.data(), 0, 0);
			buffer = m_BVBuffer;
		}

		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

		m_DeviceContext->RSSetState(m_RasterStateBV);

		buffer->setBuffer(0);
		m_BVShader->setShader();
	
		m_DeviceContext->Draw(m_BVTriangles.size(), 0);

		m_Shader->unSetShader();
		buffer->unsetBuffer(0);

		m_DeviceContext->RSSetState(m_RasterState);

		buffer = nullptr;
		m_BVTriangles.clear();
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