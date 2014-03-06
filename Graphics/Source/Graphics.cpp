#include "Graphics.h"
#include "GraphicsLogger.h"
#include "GraphicsExceptions.h"
#include "VRAMInfo.h"

#include <iostream>
#include <boost/filesystem.hpp>
#include <algorithm>

//#define TIMER_START(x) { x->Start(); }
//#define TIMER_STOP(x) { x->Stop(); double time = 0; x->GetAverageTime(time); if(time > 0) GraphicsLogger::log(GraphicsLogger::Level::INFO, std::to_string(time)); }
using namespace DirectX;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;

typedef vector<pair<IGraphics::Object2D_Id, Renderable2D>>::iterator Renderable2DIterator;
typedef vector<pair<IGraphics::InstanceId, ModelInstance>>::iterator ModelInstanceIterator;

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
	m_ScreenRenderer = nullptr;
	m_TextRenderer = nullptr;

	m_ConstantBuffer = nullptr;
	m_BVBuffer = nullptr;
	m_VSyncEnabled = false; //DEBUG
	m_NextInstanceId = 1;
	m_Next2D_ObjectId = 1;
	m_NextParticleInstanceId = 1;
	m_SelectedRenderTarget = IGraphics::RenderTarget::FINAL;
}

Graphics::~Graphics(void)
{
}

IGraphics *IGraphics::createGraphics()
{
	return new Graphics();
}

bool Graphics::initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight, bool p_Fullscreen, float p_FOV)
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
	createDefaultShaders();
	
	float nearZ = 10.0f;
	float farZ = 100000.0f;
	m_FOV = 2 * PI / 360.0f * p_FOV;
	initializeMatrices(p_ScreenWidth, p_ScreenHeight, nearZ, farZ);

	//Deferred renderer
	m_DeferredRender = new DeferredRenderer();
	m_DeferredRender->initialize(m_Device,m_DeviceContext, m_DepthStencilView,p_ScreenWidth, p_ScreenHeight,
		m_Eye, &m_ViewMatrix, &m_ProjectionMatrix, m_ShadowMapResolution, &m_SpotLights, &m_PointLights, &m_DirectionalLights, &m_ShadowMappedLight, 
		m_FOV, m_FarZ);
	
	//Forward renderer
	m_ForwardRenderer = new ForwardRendering();
	m_ForwardRenderer->init(m_Device, m_DeviceContext, m_Eye, &m_ViewMatrix, &m_ProjectionMatrix, 
		m_DepthStencilView, m_RenderTargetView);

	//Screen renderer
	m_ScreenRenderer = new ScreenRenderer();
	m_ScreenRenderer->initialize(m_Device, m_DeviceContext, &m_ViewMatrix, 
		XMFLOAT4((float)p_ScreenWidth, (float)p_ScreenHeight, 0.f, (float)p_ScreenWidth), m_DepthStencilView, m_RenderTargetView);

	//Text renderer
	m_TextRenderer = new TextRenderer();
	m_TextRenderer->initialize(m_Device, m_DeviceContext, &m_Eye, &m_ViewMatrix, &m_ProjectionMatrix,
		m_RenderTargetView);

	DebugDefferedDraw();
	setClearColor(Vector4(0.0f, 0.5f, 0.0f, 1.0f)); 
	m_BVBufferNumOfElements = 100;
	Buffer::Description buffDesc;
	buffDesc.initData = nullptr;
	buffDesc.numOfElements = m_BVBufferNumOfElements;
	buffDesc.sizeOfElement = sizeof(XMFLOAT4);
	buffDesc.type = Buffer::Type::VERTEX_BUFFER;
	buffDesc.usage = Buffer::Usage::CPU_WRITE_DISCARD;
	
	cBuffer cb;
	cb.view = m_ViewMatrix;
	cb.proj = m_ProjectionMatrix;
	cb.campos = m_Eye;

	Buffer::Description cbdesc;
	cbdesc.initData = &cb;
	cbdesc.numOfElements = 1;
	cbdesc.sizeOfElement = sizeof(cBuffer);
	cbdesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;
	cbdesc.usage = Buffer::Usage::DEFAULT;
	m_ConstantBuffer = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(cBuffer));
	
	m_BVBuffer = WrapperFactory::getInstance()->createBuffer(buffDesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(XMFLOAT4) * m_BVBufferNumOfElements);

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

	m_TextFactory.shutdown();

	if(m_SwapChain)
	{
		m_SwapChain->SetFullscreenState(false, NULL);
	}

	while(!m_ShaderList.empty())
	{
		std::map<string, Shader*>::iterator it = m_ShaderList.begin();
		string unremovedName = it->first;
		deleteShader(it->first.c_str());
	}	

	for(auto &texture : m_TextureList)
	{
		SAFE_RELEASE(texture.second);
	}
	m_TextureList.clear();
		
	while(!m_ParticleEffectDefinitionList.empty())
	{
		std::map<string, ParticleEffectDefinition::ptr>::iterator it = m_ParticleEffectDefinitionList.begin();
		string unremovedName = it->first;

		string derp = it->second->textureResourceName;

		releaseParticleEffectDefinition(unremovedName.c_str());
	}	

	while(!m_ModelList.empty())
	{
		std::map<string, ModelDefinition>::iterator it = m_ModelList.begin();

		string unremovedName = it->first;

		GraphicsLogger::log(GraphicsLogger::Level::WARNING, "Model '" + unremovedName + "' not removed properly");

		releaseModel(unremovedName.c_str());
	}

	while(!m_2D_Objects.empty())
	{
		std::map<Object2D_Id, Renderable2D>::iterator it = m_2D_Objects.begin();

		Object2D_Id unremovedId = it->first;

		GraphicsLogger::log(GraphicsLogger::Level::WARNING, "2DModel '" + std::to_string(unremovedId) + "' not removed properly");

		release2D_Model(unremovedId);
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

	SAFE_DELETE(m_ConstantBuffer);
	SAFE_DELETE(m_BVBuffer);
	
	//Deferred render
	SAFE_DELETE(m_DeferredRender);
	SAFE_DELETE(m_ForwardRenderer);
	SAFE_DELETE(m_ScreenRenderer);
	SAFE_DELETE(m_TextRenderer);

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
	m_ModelList.insert(pair<string, ModelDefinition>(p_ModelId, std::move(m_ModelFactory->getInstance()->createModel(p_Filename))));
	return true;
}

bool Graphics::releaseModel(const char* p_ResourceName)
{
	std::string resourceName(p_ResourceName);
	if(m_ModelList.count(resourceName) > 0)
	{
		for(unsigned int i = 0; i < m_ModelList.at(resourceName).numOfMaterials; i++)
		{
			m_ReleaseModelTexture(m_ModelList.at(resourceName).diffuseTexture[i].first.c_str(), m_ReleaseModelTextureUserdata);
			m_ReleaseModelTexture(m_ModelList.at(resourceName).normalTexture[i].first.c_str(), m_ReleaseModelTextureUserdata);
			m_ReleaseModelTexture(m_ModelList.at(resourceName).specularTexture[i].first.c_str(), m_ReleaseModelTextureUserdata);
		}
		m_ModelList.erase(resourceName);
		return true;
	}

	return false;
}

bool Graphics::release2D_Model(Object2D_Id p_ObjectId)
{
	if(m_2D_Objects.count(p_ObjectId) > 0)
	{
		if (m_2D_Objects.at(p_ObjectId).model->numOfMaterials == 0)
		{
			SAFE_DELETE(m_2D_Objects.at(p_ObjectId).model);
		}
		m_2D_Objects.erase(p_ObjectId);
		return true;
	}

	return false;
}

void Graphics::createShader(const char *p_shaderId, LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, ShaderType p_Type)
{
	if(m_ShaderList.count(std::string(p_shaderId)) > 0)
	{
		m_WrapperFactory->addShaderStep(m_ShaderList.at(std::string(p_shaderId)), p_Filename, nullptr, p_EntryPoint, p_ShaderModel, p_Type);
	}
	else
	{
		m_ShaderList.insert(make_pair(p_shaderId, m_WrapperFactory->createShader(p_Filename, p_EntryPoint, p_ShaderModel, p_Type)));
	}
}

void Graphics::createShader(const char *p_shaderId, LPCWSTR p_Filename, const char *p_EntryPoint,
	const char *p_ShaderModel, ShaderType p_Type, ShaderInputElementDescription *p_VertexLayout,
	unsigned int p_NumOfElements)

{
	if(!(p_Type & ShaderType::VERTEX_SHADER))
		throw ShaderException("Failed to create shader, no vertex shader defined", __LINE__, __FILE__);

	if(m_ShaderList.count(std::string(p_shaderId)) > 0)
		throw ShaderException("Failed to create shader, shader already exists", __LINE__, __FILE__);

	m_ShaderList.insert(make_pair(p_shaderId, m_WrapperFactory->createShader(p_Filename, nullptr, p_EntryPoint,
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
	std::string shaderId(p_ShaderId);
	if(m_ShaderList.count(string(shaderId)) > 0)
	{
		SAFE_DELETE(m_ShaderList.at(shaderId));
		m_ShaderList.erase(shaderId);
	}
	else
		throw GraphicsException("Failed to set delete shader: " + shaderId + " does not exist", __LINE__, __FILE__);
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

	m_TextureList.insert(make_pair(p_TextureId, resourceView));

	return true;
}

bool Graphics::releaseTexture(const char *p_TextureId)
{
	std::string textureId(p_TextureId);
	if(m_TextureList.count(textureId) > 0)
	{
		ID3D11ShaderResourceView *&m = m_TextureList.at(textureId);
		int size = calculateTextureSize(m);
		VRAMInfo::getInstance()->updateUsage(-size);

		SAFE_RELEASE(m);
		m_TextureList.erase(textureId);
		return true;
	}
	return false;
}

bool Graphics::createParticleEffectDefinition(const char * /*p_FileId*/, const char *p_FilePath)
{
	std::vector<ParticleEffectDefinition::ptr> tempList = m_ParticleFactory->createParticleEffectDefinition(p_FilePath);
	
	for (unsigned int i = 0; i < tempList.size(); i++)
	{
		m_ParticleEffectDefinitionList.insert(make_pair(tempList[i]->particleSystemName, tempList[i]));
	}

	return true;
}

bool Graphics::releaseParticleEffectDefinition(const char *p_ParticleEffectId)
{
	std::string id(p_ParticleEffectId);
	if(m_ParticleEffectDefinitionList.count(id) > 0)
	{
		m_ReleaseModelTexture(m_ParticleEffectDefinitionList.at(id)->textureResourceName.c_str(), m_ReleaseModelTextureUserdata);
		m_ParticleEffectDefinitionList.erase(id);
		return true;
	}
	else
		return false;
}

IGraphics::InstanceId Graphics::createParticleEffectInstance(const char *p_ParticleEffectId)
{
	ParticleEffectDefinition::ptr effectDef = getParticleFromList(p_ParticleEffectId);
	if (!effectDef)
	{
		GraphicsLogger::log(GraphicsLogger::Level::ERROR_L,
			"Attempting to create particle effect instance without loading the effect definition: "
			+ string(p_ParticleEffectId) );
		return -1;
	}

	ParticleInstance::ptr instance = m_ParticleFactory->createParticleInstance(effectDef);
	int id = m_NextParticleInstanceId++;

	m_ParticleEffectInstanceList.insert(make_pair(id, instance));

	return id;
}

void Graphics::releaseParticleEffectInstance(InstanceId p_ParticleEffectId)
{
	if(m_ParticleEffectInstanceList.count(p_ParticleEffectId) > 0)
		m_ParticleEffectInstanceList.erase(p_ParticleEffectId);
}

void Graphics::setParticleEffectPosition(InstanceId p_ParticleEffectId, Vector3 p_Position)
{
	if(m_ParticleEffectInstanceList.count(p_ParticleEffectId) > 0)
	{
		DirectX::XMFLOAT4 pos(p_Position.x,	p_Position.y, p_Position.z,	1.f);
		m_ParticleEffectInstanceList.at(p_ParticleEffectId)->setPosition(pos);
	}
}

void Graphics::setParticleEffectRotation(InstanceId p_ParticleEffectId, Vector3 p_Rotation)
{
	if(m_ParticleEffectInstanceList.count(p_ParticleEffectId) > 0)
	{
		DirectX::XMFLOAT3 rot(p_Rotation.x,	p_Rotation.y, p_Rotation.z);
		m_ParticleEffectInstanceList.at(p_ParticleEffectId)->setSysRotation(rot);
	}
}

void Graphics::setParticleEffectBaseColor(InstanceId p_ParticleEffectId, Vector4 p_BaseColor)
{
	if(m_ParticleEffectInstanceList.count(p_ParticleEffectId) > 0)
	{
		DirectX::XMFLOAT4 baseColor(p_BaseColor.x,	p_BaseColor.y, p_BaseColor.z, p_BaseColor.w);
		m_ParticleEffectInstanceList.at(p_ParticleEffectId)->setSysBaseColor(baseColor);
	}
}

void Graphics::updateParticles(float p_DeltaTime)
{
	for (auto& particle : m_ParticleEffectInstanceList)
	{
		particle.second->update(p_DeltaTime);
	}

	auto iter = m_ParticleEffectInstanceList.begin();
	auto endIter = m_ParticleEffectInstanceList.end();
	for (; iter != endIter; )
	{
		if (iter->second->getSeppuku() == true)
		{
			m_ParticleEffectInstanceList.erase(iter++);
		}
		else
		{
			iter++;
		}
	}
}

IGraphics::Object2D_Id Graphics::create2D_Object(Vector3 p_Position, Vector2 p_HalfSize, Vector3 p_Scale,
	float p_Rotation, const char *p_TextureId)
{
	ModelDefinition *model = m_ModelFactory->create2D_Model(p_HalfSize, p_TextureId);
	
	m_2D_Objects.insert(make_pair(m_Next2D_ObjectId, Renderable2D(std::move(model))));
	m_2D_Objects.at(m_Next2D_ObjectId).halfSize = p_HalfSize;
	set2D_ObjectPosition(m_Next2D_ObjectId, p_Position);
	set2D_ObjectScale(m_Next2D_ObjectId, p_Scale);
	set2D_ObjectRotationZ(m_Next2D_ObjectId, p_Rotation);

	return m_Next2D_ObjectId++;
}

IGraphics::Object2D_Id Graphics::create2D_Object(Vector3 p_Position, Vector3 p_Scale, float p_Rotation,
	IGraphics::Text_Id p_TextureId)
{
	ModelDefinition *model = m_ModelFactory->create2D_Model(m_TextFactory.getSRV(p_TextureId));

	m_2D_Objects.insert(make_pair(m_Next2D_ObjectId, Renderable2D(std::move(model))));
	set2D_ObjectPosition(m_Next2D_ObjectId, p_Position);
	set2D_ObjectScale(m_Next2D_ObjectId, p_Scale);
	set2D_ObjectRotationZ(m_Next2D_ObjectId, p_Rotation);

	return m_Next2D_ObjectId++;
}

IGraphics::Object2D_Id Graphics::create2D_Object(Vector3 p_Position, Vector3 p_Scale, float p_Rotation,
	const char *p_ModelDefinition)
{
	ModelDefinition *defintion = nullptr;
	for(auto &model : m_ModelList)
	{
		if(model.first == string(p_ModelDefinition))
		{
			defintion = &model.second;
			break;
		}
	}
	if(!defintion)
		throw GraphicsException("Failed to find model definition", __LINE__, __FILE__);

	m_2D_Objects.insert(make_pair(m_Next2D_ObjectId, Renderable2D(std::move(defintion))));
	set2D_ObjectPosition(m_Next2D_ObjectId, p_Position);
	set2D_ObjectScale(m_Next2D_ObjectId, p_Scale);
	set2D_ObjectRotationZ(m_Next2D_ObjectId, p_Rotation);

	return m_Next2D_ObjectId++;
}

IGraphics::Text_Id Graphics::createText(const wchar_t *p_Text, const char *p_Font, float p_FontSize, Vector4 p_FontColor, 
	Vector3 p_Position, float p_Scale, float p_Rotation)
{
	Text_Id id = m_TextFactory.createText(p_Text, p_Font, p_FontSize, p_FontColor);

	ID3D11Resource *resource;
	ID3D11Texture2D *texture;
	D3D11_TEXTURE2D_DESC textureDesc;

	m_TextFactory.getSRV(id)->GetResource(&resource);
	resource->QueryInterface(&texture);
	texture->GetDesc(&textureDesc);

	SAFE_RELEASE(texture);
	SAFE_RELEASE(resource);
	Vector2 textureSize = Vector2((float)textureDesc.Width, (float)textureDesc.Height);

	TextRenderer::TextInstance temp = TextRenderer::TextInstance(p_Position, textureSize, p_Scale, p_Rotation, 
		m_TextFactory.getSRV(id));
	m_TextRenderer->addTextObject(id, temp);
	return id;
}

IGraphics::Text_Id Graphics::createText(const wchar_t *p_Text, Vector2 p_TextureSize, const char *p_Font,
	float p_FontSize, Vector4 p_FontColor, Vector3 p_Position, float p_Scale, float p_Rotation)
{
	Text_Id id = m_TextFactory.createText(p_Text, p_TextureSize, p_Font, p_FontSize, p_FontColor);
	TextRenderer::TextInstance temp = TextRenderer::TextInstance(p_Position, p_TextureSize, p_Scale, p_Rotation, 
		m_TextFactory.getSRV(id));
	m_TextRenderer->addTextObject(id, temp);
	return id;
}

IGraphics::Text_Id Graphics::createText(const wchar_t *p_Text, Vector2 p_TextureSize, const char *p_Font,
	float p_FontSize, Vector4 p_FontColor, TEXT_ALIGNMENT p_TextAlignment, PARAGRAPH_ALIGNMENT p_ParagraphAlignment,
	WORD_WRAPPING p_WordWrapping, Vector3 p_Position, float p_Scale, float p_Rotation)
{
	Text_Id id = m_TextFactory.createText(p_Text, p_TextureSize, p_Font, p_FontSize, p_FontColor,
		p_TextAlignment, p_ParagraphAlignment, p_WordWrapping);
	TextRenderer::TextInstance temp = TextRenderer::TextInstance(p_Position, p_TextureSize, p_Scale, p_Rotation, 
		m_TextFactory.getSRV(id));
	m_TextRenderer->addTextObject(id, temp);
	return id;
}

void Graphics::render2D_Object(Object2D_Id p_Id)
{
	for(auto &object : m_2D_Objects)
	{
		if(object.first == p_Id)
		{
			m_ScreenRenderer->add2D_Object(object.second);
		}
	}
}

void Graphics::renderModel(InstanceId p_ModelId)
{
	if(m_ModelInstances.count(p_ModelId) > 0)
	{
		ModelDefinition *modelDef = getModelFromList(m_ModelInstances.at(p_ModelId).getModelName());
		if(!modelDef->isTransparent)
		{
			m_DeferredRender->addRenderable(Renderable(
				Renderable::Type::DEFERRED_OBJECT, modelDef,
				m_ModelInstances.at(p_ModelId).getWorldMatrix(), &m_ModelInstances.at(p_ModelId).getFinalTransform()));
		}
		else
		{
			m_ForwardRenderer->addRenderable(Renderable(
				Renderable::Type::FORWARD_OBJECT, modelDef,
				m_ModelInstances.at(p_ModelId).getWorldMatrix(), &m_ModelInstances.at(p_ModelId).getFinalTransform(),
				&m_ModelInstances.at(p_ModelId).getColorTone()));
		}
	}
	else
		throw GraphicsException("Failed to render model instance, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::renderSkydome(void)
{
	m_DeferredRender->renderSkyDome();
}

void Graphics::renderText(IGraphics::Text_Id p_Id)
{
	m_TextRenderer->renderTextObject(p_Id);
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
	
	XMFLOAT3 lightDirection = p_LightDirection;
	XMVECTOR lightDirectionV = XMVector3Normalize(XMLoadFloat3(&lightDirection));

	XMStoreFloat3(&l.lightDirection, lightDirectionV);
	l.spotlightAngles = XMFLOAT2(p_SpotLightAngles.x,p_SpotLightAngles.y);
	l.lightRange = p_LightRange;
	m_SpotLights.push_back(l);
}
void Graphics::useFrameDirectionalLight(Vector3 p_LightColor, Vector3 p_LightDirection, float p_Intensity)
{
	Light l;
	l.lightColor = XMFLOAT3(p_LightColor.x,p_LightColor.y,p_LightColor.z);

	XMFLOAT3 lightDirection = p_LightDirection;
	XMVECTOR lightDirectionV = XMVector3Normalize(XMLoadFloat3(&lightDirection));

	XMStoreFloat3(&l.lightDirection, lightDirectionV);
	l.lightIntensity = p_Intensity;
	if(m_ShadowMap != false)
	{
		if(m_ShadowMappedLight.lightIntensity == 0)
		{
			m_ShadowMappedLight = l;
		}
		else if(m_ShadowMappedLight.lightIntensity > l.lightIntensity)
		{
			m_DirectionalLights.push_back(l);
		}
		else
		{
			m_DirectionalLights.push_back(m_ShadowMappedLight);
			m_ShadowMappedLight = l;
		}
	}
	else
	{
		m_DirectionalLights.push_back(l);
	}
}

void Graphics::setClearColor(Vector4 p_Color)
{
	m_ClearColor[0] = p_Color.x;
	m_ClearColor[1] = p_Color.y;
	m_ClearColor[2] = p_Color.z;
	m_ClearColor[3] = p_Color.w;
}

void Graphics::drawFrame(void)
{
	if (!m_DeviceContext || !m_DeferredRender || !m_ForwardRenderer)
	{
		throw GraphicsException("", __LINE__, __FILE__);
	}
	//Update text resources
	m_TextFactory.update();


	Begin(m_ClearColor);
	m_DeferredRender->renderDeferred();
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, NULL); 
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if((int)m_SelectedRenderTarget >= 0 && (int)m_SelectedRenderTarget <= 6)
	{
		m_ShaderList.at("DebugDeferredShader")->setShader();
		m_ShaderList.at("DebugDeferredShader")->setResource(Shader::Type::PIXEL_SHADER, 0, 1, 
			m_DeferredRender->getRT(m_SelectedRenderTarget));
		m_ShaderList.at("DebugDeferredShader")->setSamplerState(Shader::Type::PIXEL_SHADER, 0, 1, m_Sampler);
		m_DeviceContext->Draw(6, 0);
		m_ShaderList.at("DebugDeferredShader")->unSetShader();
	}

	if(m_SelectedRenderTarget == IGraphics::RenderTarget::FINAL)
	{
		for (auto& particle : m_ParticleEffectInstanceList)
		{
			m_ForwardRenderer->addRenderable(particle.second);
		}
		m_ForwardRenderer->renderForward();
		m_TextRenderer->renderFrame();
		drawBoundingVolumes();
		m_ScreenRenderer->renderScreen();

	}
	End();

	m_PointLights.clear();
	m_SpotLights.clear();
	m_DirectionalLights.clear();
	m_ShadowMappedLight = Light();
}

void Graphics::setModelDefinitionTransparency(const char *p_ModelId, bool p_State)
{
	std::string modelId(p_ModelId);
	if(m_ModelList.count(modelId) > 0)
	{
		m_ModelList.at(modelId).isTransparent = p_State;
	}
	else
		throw GraphicsException("Failed to set transparency state to ModelDefinition: " + modelId + " does not exist", __LINE__, __FILE__);
}

void Graphics::animationPose(int p_Instance, const DirectX::XMFLOAT4X4* p_Pose, unsigned int p_Size)
{
	if(m_ModelInstances.count(p_Instance) > 0)
		m_ModelInstances.at(p_Instance).animationPose(p_Pose, p_Size);
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
	int id = m_NextInstanceId++;

	m_ModelInstances.insert(make_pair(id, instance));

	return id;
}

void Graphics::createSkydome(const char* p_TextureResource, float p_Radius)
{
	m_DeferredRender->createSkyDome(getTextureFromList(string(p_TextureResource)), p_Radius);
}

void Graphics::eraseModelInstance(InstanceId p_Instance)
{
	if(m_ModelInstances.count(p_Instance) > 0)
		m_ModelInstances.erase(p_Instance);
	else
		throw GraphicsException("Failed to erase model instance, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::setModelPosition(InstanceId p_Instance, Vector3 p_Position)
{
	if(m_ModelInstances.count(p_Instance) > 0)
		m_ModelInstances.at(p_Instance).setPosition(p_Position);
	else
		throw GraphicsException("Failed to set model instance position, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::setModelRotation(InstanceId p_Instance, Vector3 p_YawPitchRoll)
{
	if(m_ModelInstances.count(p_Instance) > 0)
		m_ModelInstances.at(p_Instance).setRotation(DirectX::XMFLOAT3(p_YawPitchRoll.y, p_YawPitchRoll.x, p_YawPitchRoll.z));
	else
		throw GraphicsException("Failed to set model instance position, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::setModelScale(InstanceId p_Instance, Vector3 p_Scale)
{
	if(m_ModelInstances.count(p_Instance) > 0)
		m_ModelInstances.at(p_Instance).setScale(DirectX::XMFLOAT3(p_Scale));
	else
		throw GraphicsException("Failed to set model instance scale, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::setModelColorTone(InstanceId p_Instance, Vector3 p_ColorTone)
{
	if(m_ModelInstances.count(p_Instance) > 0)
		m_ModelInstances.at(p_Instance).setColorTone(DirectX::XMFLOAT3(p_ColorTone));
	else
		throw GraphicsException("Failed to set model instance color tone, vector out of bounds.", __LINE__, __FILE__);
}

Vector3 Graphics::get2D_ObjectPosition(Object2D_Id p_Instance)
{
	if(m_2D_Objects.count(p_Instance) > 0)
		return m_2D_Objects.at(p_Instance).position;
	else
		throw GraphicsException("Failed to get model instance color tone, vector out of bounds.", __LINE__, __FILE__);
}

Vector2 Graphics::get2D_ObjectHalfSize(Object2D_Id p_Instance)
{
	if(m_2D_Objects.count(p_Instance) > 0)
		return m_2D_Objects.at(p_Instance).halfSize;
	else
		throw GraphicsException("Failed to get 2D model halfsize, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::set2D_ObjectPosition(Object2D_Id p_Instance, Vector3 p_Position)
{
	if(m_2D_Objects.count(p_Instance) > 0)
		m_2D_Objects.at(p_Instance).position = p_Position;
	else
		throw GraphicsException("Failed to set model instance color tone, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::set2D_ObjectScale(Object2D_Id p_Instance, Vector3 p_Scale)
{
	if(m_2D_Objects.count(p_Instance) > 0)
		m_2D_Objects.at(p_Instance).scale = p_Scale;
	else
		throw GraphicsException("Failed to set 2D model scale, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::set2D_ObjectRotationZ(Object2D_Id p_Instance, float p_Rotation)
{
	if(m_2D_Objects.count(p_Instance) > 0)
		XMStoreFloat4x4(&m_2D_Objects.at(p_Instance).rotation, XMMatrixRotationZ(p_Rotation)); 
	else
		throw GraphicsException("Failed to set 2D model rotation, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::set2D_ObjectLookAt(Object2D_Id p_Instance, Vector3 p_LookAt)
{
	if(m_2D_Objects.count(p_Instance) > 0)
	{
		Renderable2D& renderable = m_2D_Objects.at(p_Instance);
		XMVECTOR lookAt = XMVector3Transform(Vector3ToXMVECTOR(&p_LookAt, 1.0f), XMMatrixTranspose(XMLoadFloat4x4(&m_ViewMatrix)));
		XMMATRIX rotation = XMMatrixLookToLH(g_XMZero, lookAt,XMVectorSet(0,1,0,0));
		XMStoreFloat4x4(&renderable.rotation, XMMatrixTranspose(rotation));
	}
	else
		throw GraphicsException("Failed to set 2D model look at, vector out of bounds.", __LINE__, __FILE__);
}

void Graphics::updateText(Text_Id p_Identifier, const wchar_t *p_Text)
{
	m_TextFactory.updateText(p_Identifier, p_Text);
}

void Graphics::deleteText(Text_Id p_Identifier)
{
	m_TextFactory.deleteText(p_Identifier);
}

void Graphics::setTextColor(Text_Id p_Identifier, Vector4 p_Color)
{
	m_TextFactory.setTextColor(p_Identifier, p_Color);
}

void Graphics::setTextBackgroundColor(Text_Id p_Identifier, Vector4 p_Color)
{
	m_TextFactory.setBackgroundColor(p_Identifier, p_Color);
}

void Graphics::setTextAlignment(Text_Id p_Identifier, TEXT_ALIGNMENT p_Alignment)
{
	m_TextFactory.setTextAlignment(p_Identifier, p_Alignment);
}

void Graphics::setTextParagraphAlignment(Text_Id p_Identifier, PARAGRAPH_ALIGNMENT p_Alignment)
{
	m_TextFactory.setParagraphAlignment(p_Identifier, p_Alignment);
}

void Graphics::setTextWordWrapping(Text_Id p_Identifier, WORD_WRAPPING p_Wrapping)
{
	m_TextFactory.setWordWrapping(p_Identifier, p_Wrapping);
}

void Graphics::setTextPosition(Text_Id p_Identifier, Vector3 p_Position)
{
	m_TextRenderer->setPosition(p_Identifier, p_Position);
}

void Graphics::setTextScale(Text_Id p_Identifier, float p_Scale)
{
	m_TextRenderer->setScale(p_Identifier, p_Scale);
}

void Graphics::setTextRotation(Text_Id p_Identifier, float p_Rotation)
{
	m_TextRenderer->setRotation(p_Identifier, p_Rotation);
}

void Graphics::updateCamera(Vector3 p_Position, Vector3 p_Forward, Vector3 p_Up)
{
	XMVECTOR upVec = XMVectorSet(p_Up.x, p_Up.y, p_Up.z, 0.f);
	XMVECTOR forwardVec = XMVectorSet(p_Forward.x, p_Forward.y, p_Forward.z, 0.f);
	XMVECTOR pos = XMVectorSet(p_Position.x, p_Position.y, p_Position.z, 1.f);

	XMVECTOR flatForward = XMVectorSetY(forwardVec, 0.f);
	XMVECTOR flatUp = XMVectorSetY(upVec, 0.f);
	
	pos += flatForward * 0.f + forwardVec * 10.f + flatUp * 0.f;
	XMStoreFloat3(&m_Eye, pos);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixLookToLH(pos, forwardVec, upVec)));

	updateConstantBuffer();
	m_DeferredRender->updateCamera(m_Eye);
	m_ForwardRenderer->updateCamera(m_Eye);
}

void Graphics::addBVTriangle(Vector3 p_Corner1, Vector3 p_Corner2, Vector3 p_Corner3)
{
	if(m_SelectedRenderTarget == IGraphics::RenderTarget::FINAL)
	{
		m_BVTriangles.push_back(Vector3ToXMFLOAT4(&p_Corner1, 1.f));
		m_BVTriangles.push_back(Vector3ToXMFLOAT4(&p_Corner2, 1.f));
		m_BVTriangles.push_back(Vector3ToXMFLOAT4(&p_Corner3, 1.f));
	}
}

void Graphics::setLogFunction(clientLogCallback_t p_LogCallback)
{
	GraphicsLogger::setLogFunction(p_LogCallback);
}

void Graphics::setTweaker(TweakSettings* p_Tweaker)
{
	TweakSettings::initializeSlave(p_Tweaker);
}

void Graphics::setRenderTarget(IGraphics::RenderTarget p_RenderTarget)
{
	m_SelectedRenderTarget = p_RenderTarget;
}

void Graphics::setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void *p_Userdata)
{
	if(!m_ModelFactory)
	{
		m_ModelFactory = ModelFactory::getInstance();
		m_ModelFactory->initialize(&m_TextureList, &m_ShaderList);
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

void Graphics::renderJoint(DirectX::XMFLOAT4X4 p_World)
{
	ModelDefinition* jointDef = getModelFromList("Pivot1");
	if (jointDef)
	{
		m_DeferredRender->addRenderable(Renderable(
		Renderable::Type::DEFERRED_OBJECT, jointDef,
		p_World));
	}
}

void Graphics::enableVsync(bool p_State)
{
	m_VSyncEnabled = p_State;
}

void Graphics::enableSSAO(bool p_State)
{
	m_DeferredRender->enableSSAO(p_State);
}

void Graphics::enableShadowMap(bool p_State)
{
	m_ShadowMap = p_State;
	m_DeferredRender->enableShadowMap(p_State);
}

void Graphics::setShadowMapResolution(int p_ShadowMapResolution)
{
	m_ShadowMapResolution = p_ShadowMapResolution;
}


void Graphics::createDefaultShaders(void)
{
	createShader("DefaultDeferredShader", L"assets/shaders/GeometryPass.hlsl", "VS,PS","5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	createShader("DefaultForwardShader", L"assets/shaders/ForwardShader.hlsl", "VS,PS","5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	createShader("DefaultParticleShader", L"assets/shaders/ParticleSystem.hlsl", "VS,PS,GS", "5_0",
		ShaderType::VERTEX_SHADER | ShaderType::GEOMETRY_SHADER | ShaderType::PIXEL_SHADER);
	createShader("DefaultAnimatedShader", L"assets/shaders/AnimatedGeometryPass.hlsl",	"VS,PS","5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);

	// Debug shaders 
	createShader("DebugDeferredShader", L"assets/shaders/DebugShader.hlsl","VS,PS","5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	createShader("DebugBV_Shader", L"assets/shaders/BoundingVolume.hlsl",
		"VS,PS", "5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
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
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED;
	return D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, &featureLevel, 1, 
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

	D3D11_TEXTURE2D_DESC desc;
	backBufferPtr->GetDesc(&desc);
	int usage = VRAMInfo::getInstance()->calculateFormatUsage(desc.Format, desc.Width, desc.Height);
	VRAMInfo::getInstance()->updateUsage(usage);
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

	unsigned int size = VRAMInfo::getInstance()->calculateFormatUsage(depthBufferDesc.Format,
		depthBufferDesc.Width, depthBufferDesc.Height);
	VRAMInfo::getInstance()->updateUsage(size);

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
	m_ModelFactory->initialize(&m_TextureList, &m_ShaderList);
	m_ParticleFactory.reset(new ParticleFactory);
	m_ParticleFactory->initialize(&m_TextureList, &m_ShaderList, m_Device);
	m_TextureLoader = TextureLoader(m_Device, m_DeviceContext);
	m_TextFactory.initialize(m_Device);
}

void Graphics::initializeMatrices(int p_ScreenWidth, int p_ScreenHeight, float p_NearZ, float p_FarZ)
{
	XMFLOAT4 eye;
	XMFLOAT4 lookAt;
	XMFLOAT4 up;
	m_Eye = XMFLOAT3(0,0,-20);
	m_FarZ = p_FarZ;

	eye = XMFLOAT4(m_Eye.x,m_Eye.y,m_Eye.z,1);
	lookAt = XMFLOAT4(0,0,0,1);
	up = XMFLOAT4(0,1,0,0);
	m_ViewMatrix = XMFLOAT4X4();
	m_ProjectionMatrix = XMFLOAT4X4();

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&eye),
		XMLoadFloat4(&lookAt), XMLoadFloat4(&up))));
	XMStoreFloat4x4(&m_ProjectionMatrix, XMMatrixTranspose(XMMatrixPerspectiveFovLH(m_FOV,
		(float)p_ScreenWidth / (float)p_ScreenHeight, p_NearZ, m_FarZ)));
}

Shader *Graphics::getShaderFromList(string p_Identifier)
{
	if(m_ShaderList.count(p_Identifier) > 0)
		return m_ShaderList.at(p_Identifier);
	else
		throw GraphicsException("Failed to get shader from list, vector out of bounds:" + p_Identifier,
		__LINE__, __FILE__);
}

ModelDefinition *Graphics::getModelFromList(string p_Identifier)
{
	if(m_ModelList.count(p_Identifier) > 0)
		return &m_ModelList.at(p_Identifier);
	else
		throw GraphicsException("Failed to get model from list, vector out of bounds: " + p_Identifier,
		__LINE__, __FILE__);
}

ParticleEffectDefinition::ptr Graphics::getParticleFromList(string p_Identifier)
{
	if(m_ParticleEffectDefinitionList.count(p_Identifier) > 0)
		return m_ParticleEffectDefinitionList.at(p_Identifier);
	else
		throw GraphicsException("Failed to get particle from list, vector out of bounds:" + p_Identifier,
		__LINE__, __FILE__);
}

ID3D11ShaderResourceView *Graphics::getTextureFromList(string p_Identifier)
{
	if(m_TextureList.count(p_Identifier) > 0)
		return m_TextureList.at(p_Identifier);
	else
		throw GraphicsException("Failed to get texture from list, vector out of bounds: " + p_Identifier,
		__LINE__, __FILE__);
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
		if(m_BVTriangles.size() > m_BVBufferNumOfElements)
		{
			SAFE_DELETE(m_BVBuffer);
			VRAMInfo::getInstance()->updateUsage(-(int)(sizeof(XMFLOAT4) * m_BVBufferNumOfElements));
			m_BVBufferNumOfElements *= 2;
			if (m_BVTriangles.size() > m_BVBufferNumOfElements)
			{
				m_BVBufferNumOfElements = m_BVTriangles.size();
			}
			Buffer::Description buffDesc;
			buffDesc.initData = nullptr;
			buffDesc.numOfElements = m_BVBufferNumOfElements;
			buffDesc.sizeOfElement = sizeof(XMFLOAT4);
			buffDesc.type = Buffer::Type::VERTEX_BUFFER;
			buffDesc.usage = Buffer::Usage::CPU_WRITE_DISCARD;
	
			m_BVBuffer = WrapperFactory::getInstance()->createBuffer(buffDesc);
			VRAMInfo::getInstance()->updateUsage(sizeof(XMFLOAT4) * m_BVBufferNumOfElements);
		}

		D3D11_MAPPED_SUBRESOURCE resource;
		m_DeviceContext->Map(m_BVBuffer->getBufferPointer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		std::copy(m_BVTriangles.begin(), m_BVTriangles.end(), (XMFLOAT4*)resource.pData);
		m_DeviceContext->Unmap(m_BVBuffer->getBufferPointer(), 0);

		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

		m_DeviceContext->RSSetState(m_RasterStateBV);

		m_BVBuffer->setBuffer(0);
		m_ConstantBuffer->setBuffer(1);
		
		m_ShaderList.at("DebugBV_Shader")->setShader();
	
		m_DeviceContext->Draw(m_BVTriangles.size(), 0);

		m_ShaderList.at("DebugDeferredShader")->unSetShader();
		m_BVBuffer->unsetBuffer(0);
		m_ConstantBuffer->unsetBuffer(1);

		m_DeviceContext->RSSetState(m_RasterState);

		m_BVTriangles.clear();
	}
}

void Graphics::updateConstantBuffer()
{
	cBuffer cb;
	cb.view = m_ViewMatrix;
	cb.proj = m_ProjectionMatrix;
	cb.campos = m_Eye;

	m_DeviceContext->UpdateSubresource(m_ConstantBuffer->getBufferPointer(), NULL,NULL, &cb, sizeof(cb), NULL);
}

//TODO: Remove later
void Graphics::DebugDefferedDraw(void)
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	m_Sampler = nullptr;
	m_Device->CreateSamplerState( &sd, &m_Sampler );
}