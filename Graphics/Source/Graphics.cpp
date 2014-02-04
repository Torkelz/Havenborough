#include "Graphics.h"
#include "GraphicsLogger.h"
#include "GraphicsExceptions.h"
#include "VRAMInfo.h"

#include <iostream>
#include <boost/filesystem.hpp>

using namespace DirectX;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;

const unsigned int Graphics::m_MaxLightsPerLightInstance = 100;
Graphics::Graphics(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	
	m_SwapChain = nullptr;
	m_RenderTargetView = nullptr;
	m_Sampler = nullptr;

	m_RasterState = nullptr;
	m_RasterStateBV = nullptr;

	m_DepthStencilBuffer = nullptr;
	m_DepthStencilState = nullptr;
	m_DepthStencilView = nullptr;

	m_WrapperFactory = nullptr;
	m_ModelFactory = nullptr;

	m_DeferredRender = nullptr;
	m_ForwardRenderer = nullptr;

	m_BVBuffer = nullptr;
	m_BVShader = nullptr;
	m_Shader = nullptr;
	m_VSyncEnabled = false; //DEBUG
	m_NextInstanceId = 1;
	m_NextParticleInstanceId = 1;
	m_SelectedRenderTarget = 3;
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

	initializeFactories();

	m_TextureLoader = TextureLoader(m_Device, m_DeviceContext);

	initializeMatrices(p_ScreenWidth, p_ScreenHeight);

	//Deferred Render
	m_DeferredRender = new DeferredRenderer();
	m_DeferredRender->initialize(m_Device,m_DeviceContext, m_DepthStencilView,p_ScreenWidth, p_ScreenHeight,
		&m_Eye, &m_ViewMatrix, &m_ProjectionMatrix, &m_SpotLights, &m_PointLights, &m_DirectionalLights,
		m_MaxLightsPerLightInstance);
	
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

	VRAMInfo::getInstance()->updateUsage(sizeof(XMFLOAT4) * m_BVBufferNumOfElements);

	m_BVShader = WrapperFactory::getInstance()->createShader(L"../../Graphics/Source/DeferredShaders/BoundingVolume.hlsl",
		"VS,PS", "5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);

	m_ForwardRenderer = new ForwardRendering();
	m_ForwardRenderer->init(m_Device, m_DeviceContext, &m_Eye, &m_ViewMatrix, &m_ProjectionMatrix, 
		m_DepthStencilView, m_RenderTargetView);

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
	
	while (!m_ParticleEffectDefinitionList.empty())
	{
		string unremovedName = m_ParticleEffectDefinitionList.front().first;

		GraphicsLogger::log(GraphicsLogger::Level::WARNING, "Particle '" + unremovedName + "' not removed properly");

		releaseParticleEffectDefinition(unremovedName.c_str());
	}	

	while (!m_ModelList.empty())
	{
		string unremovedName = m_ModelList.front().first;

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

	m_Shader = nullptr;
	SAFE_DELETE(m_BVBuffer);
	SAFE_DELETE(m_BVShader);
	
	//Deferred render
	SAFE_DELETE(m_DeferredRender);
	SAFE_DELETE(m_ForwardRenderer);
	//Clear lights
	m_PointLights.clear();
	m_SpotLights.clear();
	m_DirectionalLights.clear();

	m_PointLights.shrink_to_fit();
	m_SpotLights.shrink_to_fit();
	m_DirectionalLights.shrink_to_fit();

	VRAMInfo::getInstance()->updateUsage(-(int)(sizeof(XMFLOAT4) * m_BVBufferNumOfElements));
}

void IGraphics::deleteGraphics(IGraphics *p_Graphics)
{
	p_Graphics->shutdown();
	SAFE_DELETE(p_Graphics);
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

void Graphics::linkShaderToModel(const char *p_ShaderId, const char *p_ModelId)
{
	ModelDefinition *model = nullptr;
	model = getModelFromList(p_ModelId);
	if(model)
		model->shader = getShaderFromList(p_ShaderId);
}

void Graphics::linkShaderToParticles(const char *p_ShaderId, const char *p_ParticlesId)
{
	ParticleEffectDefinition::ptr particles = getParticleFromList(p_ParticlesId);
	if(particles)
		particles->shader = getShaderFromList(p_ShaderId);
}

void Graphics::deleteShader(const char *p_ShaderId)
{
	for(auto &s : m_ShaderList)
	{
		if(s.first.compare(p_ShaderId) == 0 )
		{
			SAFE_DELETE(s.second);
			std::swap(s, m_ShaderList.back());
			m_ShaderList.pop_back();
			return;
		}
	}
	string error = p_ShaderId;
	throw GraphicsException("Failed to set delete shader: " + error + " does not exist", __LINE__, __FILE__);
}

bool Graphics::createTexture(const char *p_TextureId, const char *p_Filename)
{
	ID3D11ShaderResourceView *resourceView = m_TextureLoader.createTextureFromFile(p_Filename);
	if(!resourceView)
	{
		return false;
	}

	int size = calculateTextureSize(resourceView);
	VRAMInfo::getInstance()->updateUsage(size);

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
			VRAMInfo::getInstance()->updateUsage(-size);

			SAFE_RELEASE(m);
			m_TextureList.erase(it);
			return true;
		}
	}

	return false;
}

bool Graphics::createParticleEffectDefinition(const char *p_ParticleEffectId, const char *p_Filename)
{
	ParticleEffectDefinition::ptr temp = m_ParticleFactory->createParticleEffectDefinition(p_Filename, p_ParticleEffectId);

	m_ParticleEffectDefinitionList.push_back(make_pair(p_ParticleEffectId,temp));
	
	return true;
}

bool Graphics::releaseParticleEffectDefinition(const char *p_ParticleEffectId)
{
	auto it = std::find_if(m_ParticleEffectDefinitionList.begin(), m_ParticleEffectDefinitionList.end(),
		[p_ParticleEffectId] (const pair<string, ParticleEffectDefinition::ptr>& p_Effect)
		{
			return p_Effect.first == p_ParticleEffectId;
		});

	if (it != m_ParticleEffectDefinitionList.end())
	{
		m_ReleaseModelTexture(it->second->textureResourceName.c_str(), m_ReleaseModelTextureUserdata);
		m_ParticleEffectDefinitionList.erase(it);
		return true;
	}

	return false;
}

IGraphics::InstanceId Graphics::createParticleEffectInstance(const char *p_ParticleEffectId)
{
	ParticleEffectDefinition::ptr effectDef = getParticleFromList(p_ParticleEffectId);
	if (!effectDef)
	{
		GraphicsLogger::log(GraphicsLogger::Level::ERROR_L,
			"Attempting to create particle effect instance without loading the effect definition: "
			+ string(p_ParticleEffectId));
		return -1;
	}

	ParticleInstance::ptr instance = m_ParticleFactory->createParticleInstance(effectDef);
	int id = m_NextParticleInstanceId++;

	m_ParticleEffectInstanceList.push_back(make_pair(id, instance));

	return id;
}

void Graphics::releaseParticleEffectInstance(InstanceId p_ParticleEffectId)
{
	auto it = std::find_if(m_ParticleEffectInstanceList.begin(), m_ParticleEffectInstanceList.end(),
		[p_ParticleEffectId] (const pair<InstanceId, ParticleInstance::ptr>& p_Effect)
		{
			return p_Effect.first == p_ParticleEffectId;
		});
	if (it != m_ParticleEffectInstanceList.end())
	{
		m_ParticleEffectInstanceList.erase(it);
	}
}

void Graphics::setParticleEffectPosition(InstanceId p_ParticleEffectId, Vector3 p_Position)
{
	auto it = std::find_if(m_ParticleEffectInstanceList.begin(), m_ParticleEffectInstanceList.end(),
		[p_ParticleEffectId] (const pair<InstanceId, ParticleInstance::ptr>& p_Effect)
		{
			return p_Effect.first == p_ParticleEffectId;
		});
	if (it != m_ParticleEffectInstanceList.end())
	{
		DirectX::XMFLOAT4 pos(
			p_Position.x,
			p_Position.y,
			p_Position.z,
			1.f);
		it->second->setPosition(pos);
	}
}

void Graphics::updateParticles(float p_DeltaTime)
{
	for (auto& particle : m_ParticleEffectInstanceList)
	{
		particle.second->update(p_DeltaTime);
	}
}

void Graphics::renderModel(InstanceId p_ModelId)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_ModelId)
		{
			ModelDefinition *modelDef = getModelFromList(inst.second.getModelName());
			if(!modelDef->isTransparent)
			{
				m_DeferredRender->addRenderable(Renderable(
					Renderable::Type::DEFERRED_OBJECT, modelDef,
					inst.second.getWorldMatrix(), &inst.second.getFinalTransform()));
			}
			else
			{
				m_ForwardRenderer->addRenderable(Renderable(
					Renderable::Type::FORWARD_OBJECT, modelDef,
					inst.second.getWorldMatrix(), &inst.second.getFinalTransform(),
					&inst.second.getColorTone()));
			}
			
			return;
		}
	}
	throw GraphicsException("Failed to render model instance, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::renderSkydome()
{
	m_DeferredRender->renderSkyDome();
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

void Graphics::drawFrame()
{
	if (!m_DeviceContext || !m_DeferredRender || !m_ForwardRenderer)
	{
		throw GraphicsException("", __LINE__, __FILE__);
	}

	Begin(m_ClearColor);

	m_DeferredRender->renderDeferred();

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, NULL); 
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if(m_SelectedRenderTarget >= 0 && m_SelectedRenderTarget <=3)
	{
		m_Shader->setShader();
		m_Shader->setResource(Shader::Type::PIXEL_SHADER, 0, 1, m_DeferredRender->getRT(m_SelectedRenderTarget));
		m_Shader->setSamplerState(Shader::Type::PIXEL_SHADER, 0, 1, m_Sampler);
		m_DeviceContext->Draw(6, 0);
		
		m_Shader->unSetShader();
	}

	for (auto& particle : m_ParticleEffectInstanceList)
	{
		m_ForwardRenderer->addRenderable(particle.second);
	}
	m_ForwardRenderer->renderForward();

	drawBoundingVolumes();

	End();

	//Delete lights
	m_PointLights.clear();
	m_SpotLights.clear();
	m_DirectionalLights.clear();
}

void Graphics::setModelDefinitionTransparency(const char *p_ModelId, bool p_State)
{
	
	for(auto &model : m_ModelList)
	{
		if(model.first == string(p_ModelId))
		{
			model.second.isTransparent = p_State;
			return;
		}
	}
	string error = p_ModelId;
	throw GraphicsException("Failed to set transparency state to ModelDefinition: " + error + " does not exist", __LINE__, __FILE__);
}

void Graphics::updateAnimations(float p_DeltaTime)
{
	for (auto& model : m_ModelInstances)
	{
		ModelDefinition* modelDef = getModelFromList(model.second.getModelName());
		if (modelDef->isAnimated)
		{
			model.second.m_Animation.updateAnimation(p_DeltaTime);
			const std::vector<XMFLOAT4X4>& animationData = model.second.m_Animation.getFinalTransform();
			animationPose(model.first, animationData.data(), animationData.size());
		}
	}
}

void Graphics::playAnimation(int p_Instance, const char* p_ClipName, bool p_Override)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.m_Animation.playClip(p_ClipName, p_Override);
			break;
		}
	}
}

void Graphics::queueAnimation(int p_Instance, const char* p_ClipName)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.m_Animation.queueClip(p_ClipName);
			break;
		}
	}
}
void Graphics::changeAnimationWeight(int p_Instance, int p_Track, float p_Weight)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.m_Animation.changeWeight(p_Track, p_Weight);
			break;
		}
	}
}

void Graphics::animationPose(int p_Instance, const DirectX::XMFLOAT4X4* p_Pose, unsigned int p_Size)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.animationPose(p_Pose, p_Size);
			break;
		}
	}
}

int Graphics::getVRAMUsage(void)
{
	return VRAMInfo::getInstance()->getUsage();
}

IGraphics::InstanceId Graphics::createModelInstance(const char *p_ModelId)
{
	ModelDefinition *modelDef = getModelFromList(p_ModelId);
	if (modelDef == nullptr)
	{
		GraphicsLogger::log(GraphicsLogger::Level::ERROR_L, "Attempting to create model instance without loading the model definition: " +
			string(p_ModelId));
		return -1;
	}

	ModelInstance instance;
	instance.setModelName(p_ModelId);
	instance.setPosition(XMFLOAT3(0.f, 0.f, 0.f));
	instance.setRotation(XMFLOAT3(0.f, 0.f, 0.f));
	instance.setScale(XMFLOAT3(1.f, 1.f, 1.f));
	if (modelDef->isAnimated)
	{
		instance.m_Animation.setAnimationData(modelDef->animationData);
	}
	int id = m_NextInstanceId++;

	m_ModelInstances.push_back(make_pair(id, instance));

	return id;
}

void Graphics::createSkydome(const char* p_TextureResource, float p_Radius)
{
	m_DeferredRender->createSkyDome(getTextureFromList(string(p_TextureResource)), p_Radius);
}

void Graphics::eraseModelInstance(InstanceId p_Instance)
{
	for (unsigned int i = 0; i < m_ModelInstances.size(); i++)
	{
		if (m_ModelInstances.at(i).first == p_Instance)
		{
			m_ModelInstances.erase(m_ModelInstances.begin() + i);
			return;
		}
	}
	throw GraphicsException("Failed to erase model instance, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::setModelPosition(InstanceId p_Instance, Vector3 p_Position)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.setPosition(Vector3ToXMFLOAT3(&p_Position));
			return;
		}
	}
	throw GraphicsException("Failed to set model instance position, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::setModelRotation(InstanceId p_Instance, Vector3 p_YawPitchRoll)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.setRotation(DirectX::XMFLOAT3(p_YawPitchRoll.y, p_YawPitchRoll.x, p_YawPitchRoll.z));
			return;
		}
	}
	throw GraphicsException("Failed to set model instance position, vector out of bounds.", __LINE__, __FILE__);

}

void Graphics::setModelScale(InstanceId p_Instance, Vector3 p_Scale)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.setScale(DirectX::XMFLOAT3(p_Scale));
			return;
		}
	}
	throw GraphicsException("Failed to set model instance scale, vector out of bounds.", __LINE__, __FILE__);

}

void Graphics::setModelColorTone(InstanceId p_Instance, Vector3 p_ColorTone)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			inst.second.setColorTone(DirectX::XMFLOAT3(p_ColorTone));
			return;
		}
	}
	throw GraphicsException("Failed to set model instance color tone, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::applyIK_ReachPoint(InstanceId p_Instance, const char* p_IKGroupName, Vector3 p_Target)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			const ModelDefinition* modelDef = getModelFromList(inst.second.getModelName());
			//ModelDefinition* modelDef = getModelFromList(inst.second.getModelName());
			std::string tempStr(p_IKGroupName);

			// If an illegal string has been put in, just shoot in the default animation.
			// The show must go on!
			if( modelDef->animationData->ikGroups.find(p_IKGroupName) == modelDef->animationData->ikGroups.end() )
			{
				tempStr = "default";
			}

			inst.second.m_Animation.applyIK_ReachPoint(modelDef->animationData->ikGroups.at(p_IKGroupName), p_Target, inst.second.getWorldMatrix());
			const std::vector<XMFLOAT4X4>& animationData = inst.second.m_Animation.getFinalTransform();
			animationPose(p_Instance, animationData.data(), animationData.size());
			break;
		}
	}
}

Vector3 Graphics::getJointPosition(InstanceId p_Instance, const char* p_Joint)
{
	for (auto& inst : m_ModelInstances)
	{
		if (inst.first == p_Instance)
		{
			const ModelDefinition* modelDef = getModelFromList(inst.second.getModelName());
			XMFLOAT3 position = inst.second.m_Animation.getJointPos(p_Joint, inst.second.getWorldMatrix());
			
			return position;
		}
	}

	throw InvalidArgumentGraphicsException("Model instance does not exist", __LINE__, __FILE__);
}

void Graphics::updateCamera(Vector3 p_Position, Vector3 p_Forward, Vector3 p_Up)
{
	XMVECTOR upVec = XMLoadFloat3(&XMFLOAT3(p_Up));
	XMVECTOR forwardVec = XMLoadFloat3(&XMFLOAT3(p_Forward));
	XMVECTOR pos = XMLoadFloat3(&XMFLOAT3(p_Position));

	XMVECTOR flatForward = XMVectorSetY(forwardVec, 0.f);
	XMVECTOR flatUp = XMVectorSetY(upVec, 0.f);
	
	pos += flatForward * 5.f + forwardVec * 20.f + flatUp * 35.f;
	XMStoreFloat3(&m_Eye, pos);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixLookToLH(pos, forwardVec, upVec)));
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

void Graphics::setRenderTarget(int p_RenderTarget)
{
	m_SelectedRenderTarget = p_RenderTarget;
}

void Graphics::setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void *p_Userdata)
{
	if(!m_ModelFactory)
	{
		m_ModelFactory = ModelFactory::getInstance();
		m_ModelFactory->initialize(&m_TextureList);
		m_ModelFactory->setLoadModelTextureCallBack(p_LoadModelTexture, p_Userdata);
	}
	m_ModelFactory->setLoadModelTextureCallBack(p_LoadModelTexture, p_Userdata);
	m_ParticleFactory->setLoadParticleTextureCallBack(p_LoadModelTexture, p_Userdata);
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
	rasterDesc.CullMode = D3D11_CULL_NONE;
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

void Graphics::initializeFactories(void)
{
	//Note this is the only time initialize should be called.
	WrapperFactory::initialize(m_Device, m_DeviceContext);
	m_WrapperFactory = WrapperFactory::getInstance();
	VRAMInfo::getInstance();
	m_ModelFactory = ModelFactory::getInstance();
	m_ModelFactory->initialize(&m_TextureList);
	m_ParticleFactory.reset(new ParticleFactory);
	m_ParticleFactory->initialize(&m_TextureList, m_Device);
	m_TextureLoader = TextureLoader(m_Device, m_DeviceContext);
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

ParticleEffectDefinition::ptr Graphics::getParticleFromList(string p_Identifier)
{
	for(auto & s : m_ParticleEffectDefinitionList)
	{
		if(s.first == p_Identifier)
		{
			return s.second;
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

	return VRAMInfo::getInstance()->calculateFormatUsage(textureDesc.Format, textureDesc.Width, textureDesc.Height);
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
			VRAMInfo::getInstance()->updateUsage(-(int)(sizeof(XMFLOAT4) * m_BVBufferNumOfElements));
			m_BVBufferNumOfElements = m_BVTriangles.size() + 1;
			Buffer::Description buffDesc;
			buffDesc.initData = &m_BVTriangles;
			buffDesc.numOfElements = m_BVBufferNumOfElements;
			buffDesc.sizeOfElement = sizeof(XMFLOAT4);
			buffDesc.type = Buffer::Type::VERTEX_BUFFER;
			buffDesc.usage = Buffer::Usage::DEFAULT;
	
			buffer = WrapperFactory::getInstance()->createBuffer(buffDesc);
			VRAMInfo::getInstance()->updateUsage(sizeof(XMFLOAT4) * m_BVBufferNumOfElements);
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
