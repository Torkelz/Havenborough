#include "DeferredRenderer.h"

#include "ModelBinaryLoader.h"
#include "Utilities/MemoryUtil.h"
#include "GraphicsExceptions.h"
#include "VRAMInfo.h"
#include "WrapperFactory.h"

#include <TweakSettings.h>

#include <algorithm>	// std::sort
#include <iterator>     // std::back_inserter
//#include <random>


using std::vector;
using namespace DirectX;


DeferredRenderer::DeferredRenderer()


{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_DepthStencilView = nullptr;


	m_SpotLights = nullptr;
	m_PointLights = nullptr;
	m_DirectionalLights = nullptr;


	m_RT[IGraphics::RenderTarget::DIFFUSE] = nullptr;
	m_RT[IGraphics::RenderTarget::NORMAL] = nullptr;
	m_RT[IGraphics::RenderTarget::W_POSITION] = nullptr;
	m_RT[IGraphics::RenderTarget::SSAO] = nullptr;
	m_RT[IGraphics::RenderTarget::FINAL] = nullptr;


	m_SRV["Diffuse"] = nullptr;
	m_SRV["Normal"] = nullptr;
	m_SRV["Light"] = nullptr;
	m_SRV["WPosition"] = nullptr;
	m_SRV["SSAO"] = nullptr;
	m_SRV["SSAO_RandomVec"] = nullptr;


	m_Sampler["Default"] = nullptr;
	m_Shader["PointLight"] = nullptr;
	m_Shader["SpotLight"] = nullptr;
	m_Shader["DirectionalLight"] = nullptr;
	m_Shader["SSAO"] = nullptr;
	m_Shader["SSAO_Blur"] = nullptr;


	m_Buffer["PointLightModel"] = nullptr;
	m_Buffer["SpotLightModel"] = nullptr;
	m_Buffer["DirectionalLightModel"] = nullptr;


	m_Buffer["DefaultConstant"] = nullptr;
	m_Buffer["ObjectConstant"] = nullptr;
	m_Buffer["AnimatedConstant"] = nullptr;
	m_Buffer["WorldInstance"] = nullptr;
	m_Shader["IGeometry"] = nullptr;


	m_Buffer["LightConstant"] = nullptr;
	m_Buffer["SSAOConstant"] = nullptr;
	m_Buffer["SSAOConstant_Blur"] = nullptr;


	m_ViewMatrix = nullptr;
	m_ProjectionMatrix = nullptr;

	m_RasterState = nullptr;
	m_DepthState = nullptr;
	m_BlendState = nullptr;
	m_BlendState2 = nullptr;


	m_SkyDome = nullptr;
}


DeferredRenderer::~DeferredRenderer(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_DepthStencilView = nullptr;


	m_SpotLights = nullptr;
	m_PointLights = nullptr;
	m_DirectionalLights = nullptr;


	m_ViewMatrix = nullptr;
	m_ProjectionMatrix = nullptr;

	for(auto &rt : m_RT)
		SAFE_RELEASE(rt.second);
	m_RT.clear();


	for(auto &srv : m_SRV)
		SAFE_RELEASE(srv.second);
	m_SRV.clear();


	for(auto &sampler : m_Sampler)
		SAFE_RELEASE(sampler.second);
	m_Sampler.clear();


	for(auto &shader : m_Shader)
		SAFE_DELETE(shader.second);
	m_Shader.clear();


	for(auto &buffer : m_Buffer)
		SAFE_DELETE(buffer.second);
	m_Buffer.clear();


	SAFE_RELEASE(m_BlendState);
	SAFE_RELEASE(m_BlendState2);


	SAFE_RELEASE(m_RasterState);
	SAFE_RELEASE(m_DepthState);
	SAFE_DELETE(m_SkyDome);
}


void DeferredRenderer::initialize(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext,
								  ID3D11DepthStencilView *p_DepthStencilView, unsigned int p_ScreenWidth, unsigned int p_ScreenHeight,
								  DirectX::XMFLOAT3 p_CameraPosition, DirectX::XMFLOAT4X4 *p_ViewMatrix,	DirectX::XMFLOAT4X4 *p_ProjectionMatrix,
								  std::vector<Light> *p_SpotLights, std::vector<Light> *p_PointLights, std::vector<Light> *p_DirectionalLights,
								  unsigned int p_MaxLightsPerLightInstance, float p_FOV, float p_FarZ)
{
	m_Device			= p_Device;
	m_DeviceContext		= p_DeviceContext;
	m_DepthStencilView	= p_DepthStencilView;


	m_CameraPosition	= p_CameraPosition;
	m_ViewMatrix		= p_ViewMatrix;
	m_ProjectionMatrix	= p_ProjectionMatrix;


	m_SpotLights = p_SpotLights;
	m_PointLights = p_PointLights;
	m_DirectionalLights = p_DirectionalLights;
	m_RenderSkyDome = false;
	m_MaxLightsPerLightInstance = p_MaxLightsPerLightInstance;


	//Create render targets with the size of screen width and screen height
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.Width				= p_ScreenWidth;
	desc.Height				= p_ScreenHeight;
	desc.MipLevels			= 1;
	desc.ArraySize			= 1;
	desc.Format				= DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count	= 1;
	desc.Usage				= D3D11_USAGE_DEFAULT;
	desc.BindFlags			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;


	m_FOV = p_FOV;
	m_FarZ = p_FarZ;
	m_ScreenWidth = (float)p_ScreenWidth;
	m_ScreenHeight = (float)p_ScreenHeight;


	if(!m_Device || !m_DeviceContext)
		throw DeferredRenderException("Failed to initialize deferred renderer, nullpointers not allowed",
		__LINE__, __FILE__);

	createRenderTargets(desc);


	createRandomTexture(256);


	createShaderResourceViews(desc);


	createShaders();


	loadLightModels();


	// Create sampler state and blend state for Alpha rendering.
	createSamplerState();


	createBlendStates();
	createLightStates();
	createBuffers();


	TweakSettings* settings = TweakSettings::getInstance();

	settings->setSetting("ssao.radius", 5.f);
	settings->setListener("ssao.radius", std::function<void(float)>(
		[&] (float)
		{
			updateSSAO_VarConstantBuffer();
		}
	));

	settings->setSetting("ssao.epsilon", 4.f);
	settings->setListener("ssao.epsilon", std::function<void(float)>(
		[&] (float)
		{
			updateSSAO_VarConstantBuffer();
		}
	));

	settings->setSetting("ssao.fadeEnd", 10.f);
	settings->setListener("ssao.fadeEnd", std::function<void(float)>(
		[&] (float)
		{
			updateSSAO_VarConstantBuffer();
		}
	));

	settings->setSetting("ssao.fadeStart", 2.f);
	settings->setListener("ssao.fadeStart", std::function<void(float)>(
		[&] (float)
		{
			updateSSAO_VarConstantBuffer();
		}
	));
}


void DeferredRenderer::renderDeferred()
{
	// Clear render targets.
	clearRenderTargets();


	// Update constant buffer and render
	if(m_Objects.size() > 0)
	{
		updateConstantBuffer();
		renderGeometry();
		//renderSSAO();
		//blurSSAO();
		renderLighting();
	}
	m_RenderSkyDome = false;
}


void DeferredRenderer::renderGeometry()
{
	unsigned int nrRT = 3;
	ID3D11RenderTargetView *rtv[] = {
		m_RT[IGraphics::RenderTarget::DIFFUSE], m_RT[IGraphics::RenderTarget::NORMAL], m_RT[IGraphics::RenderTarget::W_POSITION]
	};
	// Set the render targets.
	m_DeviceContext->OMSetRenderTargets(nrRT, rtv, m_DepthStencilView);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// The textures will be needed to be grabbed from the model later.
	std::vector<std::vector<Renderable>> instancedModels;
	std::vector<Renderable> animatedOrSingle;
	
	SortRenderables(animatedOrSingle, instancedModels);

	m_Buffer["DefaultConstant"]->setBuffer(0);
	m_DeviceContext->PSSetSamplers(0,1,&m_Sampler["Default"]);

	for( auto &animation : animatedOrSingle )
		renderObject(animation);
	
	for( auto &k : instancedModels)
		RenderObjectsInstanced(k);


	m_DeviceContext->PSSetSamplers(0,0,0);
	m_Buffer["DefaultConstant"]->unsetBuffer(0);


	// Unset render targets.
	m_DeviceContext->OMSetRenderTargets(0, 0, 0);
}


void DeferredRenderer::renderSSAO(void)
{
	m_DeviceContext->OMSetRenderTargets(1, &m_RT[IGraphics::RenderTarget::SSAO], 0);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	m_Shader["SSAO"]->setShader();
	m_Buffer["DefaultConstant"]->setBuffer(0);
	m_Buffer["SSAOConstant"]->setBuffer(1);


	ID3D11SamplerState *samplers[] =
	{
		m_Sampler["SSAO_NormalDepth"],
		m_Sampler["SSAO_RandomVec"]
	};
	m_DeviceContext->PSSetSamplers(0, 2,  samplers);


	ID3D11ShaderResourceView *srvs[] =
	{
		m_SRV["Normal"],
		m_SRV["SSAO_RandomVec"]
	};
	m_DeviceContext->PSSetShaderResources(0, 2, srvs);


	m_DeviceContext->Draw(6, 0);


	ID3D11ShaderResourceView *nullSrvs[] = { 0, 0 };
	m_DeviceContext->PSSetShaderResources(0, 2, nullSrvs);


	ID3D11SamplerState *nullSamplers[] = { 0, 0	};
	m_DeviceContext->PSSetSamplers(0, 2, nullSamplers);


	m_Buffer["SSAOConstant"]->unsetBuffer(1);
	m_Buffer["DefaultConstant"]->unsetBuffer(0);
	m_Shader["SSAO"]->unSetShader();
	m_DeviceContext->OMSetRenderTargets(0, 0, 0);
}


void DeferredRenderer::blurSSAO(void)
{
	for(int i = 0; i < 2; i++) //TODO: Should be 4 passes
	{
		SSAO_PingPong(m_SRV["SSAO"], m_RT[IGraphics::RenderTarget::FINAL], false);
		SSAO_PingPong(m_SRV["Light"], m_RT[IGraphics::RenderTarget::SSAO], true);
	}
	float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_DeviceContext->ClearRenderTargetView(m_RT[IGraphics::RenderTarget::FINAL], color);
}


void DeferredRenderer::SSAO_PingPong(ID3D11ShaderResourceView *inputSRV, ID3D11RenderTargetView *outputTarget,
									 bool p_HorizontalBlur)
{
	updateSSAO_BlurConstantBuffer(p_HorizontalBlur);


	m_DeviceContext->OMSetRenderTargets(1, &outputTarget, 0);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	m_Shader["SSAO_Blur"]->setShader();
	m_Buffer["SSAOConstant_Blur"]->setBuffer(0);


	m_DeviceContext->PSSetSamplers(0, 1, &m_Sampler["SSAO_Blur"]);


	ID3D11ShaderResourceView *srvs[] =
	{
		m_SRV["Normal"],
		inputSRV
	};
	m_DeviceContext->PSSetShaderResources(0, 2, srvs);


	m_DeviceContext->Draw(6, 0);


	ID3D11ShaderResourceView *nullSrvs[] = { 0, 0 };
	m_DeviceContext->PSSetShaderResources(0, 2, nullSrvs);


	ID3D11SamplerState *nullSamplers[] = { 0 };
	m_DeviceContext->PSSetSamplers(0, 1, nullSamplers);


	m_Buffer["SSAOConstant_Blur"]->unsetBuffer(0);
	m_Shader["SSAO_Blur"]->unSetShader();
	m_DeviceContext->OMSetRenderTargets(0, 0, 0);
}


void DeferredRenderer::updateSSAO_BlurConstantBuffer(bool p_HorizontalBlur)
{
	cSSAO_BlurBuffer blurBuffer;
	blurBuffer.horizontalBlur = p_HorizontalBlur;
	blurBuffer.texelWidth = 1.0f / m_ScreenWidth;
	blurBuffer.texelHeight = 1.0f / m_ScreenHeight;
	m_DeviceContext->UpdateSubresource(m_Buffer["SSAOConstant_Blur"]->getBufferPointer(), NULL, nullptr, &blurBuffer,
		NULL, NULL);
}

void DeferredRenderer::updateSSAO_VarConstantBuffer()
{
	cSSAO_Buffer ssaoBuffer;
	float aspect = m_ScreenWidth / m_ScreenHeight;
	float halfHeight = m_FarZ * std::tanf(0.5f * m_FOV);
	float halfWidth = aspect * halfHeight;

	ssaoBuffer.corners[0] = DirectX::XMFLOAT4(-halfWidth, -halfHeight, m_FarZ, 0);
	ssaoBuffer.corners[1] = DirectX::XMFLOAT4(-halfWidth, +halfHeight, m_FarZ, 0);
	ssaoBuffer.corners[2] = DirectX::XMFLOAT4(+halfWidth, +halfHeight, m_FarZ, 0);
	ssaoBuffer.corners[3] = DirectX::XMFLOAT4(+halfWidth, -halfHeight, m_FarZ, 0);
	buildSSAO_OffsetVectors(ssaoBuffer);

	ssaoBuffer.occlusionRadius	= 5.0f;
	ssaoBuffer.surfaceEpsilon	= 4.0f;
	ssaoBuffer.occlusionFadeEnd	= 10.0f;
	ssaoBuffer.occlusionFadeStart = 2.0f;

	TweakSettings* settings = TweakSettings::getInstance();
	settings->querySetting("ssao.radius", ssaoBuffer.occlusionRadius);
	settings->querySetting("ssao.epsilon", ssaoBuffer.surfaceEpsilon);
	settings->querySetting("ssao.fadeEnd", ssaoBuffer.occlusionFadeEnd);
	settings->querySetting("ssao.fadeStart", ssaoBuffer.occlusionFadeStart);

	m_DeviceContext->UpdateSubresource(m_Buffer["SSAOConstant"]->getBufferPointer(),
		NULL, nullptr, &ssaoBuffer, NULL, NULL);
}

void DeferredRenderer::renderLighting()
{
	// Store previous States to be set when we exit the method.
	ID3D11RasterizerState *previousRasterState;
	ID3D11DepthStencilState *previousDepthState;
	m_DeviceContext->RSGetState(&previousRasterState);
	m_DeviceContext->OMGetDepthStencilState(&previousDepthState,0);


	// Collect the shader resources in an array and create a clear array.
	ID3D11ShaderResourceView *srvs[] = {m_SRV["WPosition"], m_SRV["Normal"], m_SRV["Diffuse"], m_SRV["SSAO"]};
	ID3D11ShaderResourceView *nullsrvs[] = {0,0,0,0};


	// Set texture sampler.
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	UINT sampleMask = 0xffffffff;
	m_DeviceContext->PSSetShaderResources(0, 4, srvs);


	////Select the third render target[3]
	m_DeviceContext->OMSetRenderTargets(1, &m_RT[IGraphics::RenderTarget::FINAL], m_DepthStencilView); 
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	m_DeviceContext->RSSetState(m_RasterState);
	m_DeviceContext->OMSetDepthStencilState(m_DepthState,0);
	m_DeviceContext->OMSetBlendState(m_BlendState, blendFactor, sampleMask);


	//Set constant data
	m_Buffer["DefaultConstant"]->setBuffer(0);


	//		Render PointLights
	renderLight(m_Shader["PointLight"], m_Buffer["PointLightModel"], m_PointLights);
	//		Render SpotLights
	renderLight(m_Shader["SpotLight"], m_Buffer["SpotLightModel"], m_SpotLights);
	//		Render DirectionalLights
	m_DeviceContext->OMSetRenderTargets(1, &m_RT[IGraphics::RenderTarget::FINAL],0);
	renderLight(m_Shader["DirectionalLight"], m_Buffer["DirectionalLightModel"], m_DirectionalLights);

	m_Buffer["DefaultConstant"]->unsetBuffer(0);


	if(m_SkyDome && m_RenderSkyDome)
		m_SkyDome->RenderSkyDome(m_RT[IGraphics::RenderTarget::FINAL], m_DepthStencilView, m_Buffer["DefaultConstant"]);


	m_DeviceContext->PSSetShaderResources(0, 4, nullsrvs);
	m_DeviceContext->OMSetRenderTargets(0, 0, 0);
	m_DeviceContext->RSSetState(previousRasterState);
	m_DeviceContext->OMSetDepthStencilState(previousDepthState,0);
	m_DeviceContext->OMSetBlendState(0, blendFactor, sampleMask);
	SAFE_RELEASE(previousRasterState);
	SAFE_RELEASE(previousDepthState);
}


void DeferredRenderer::addRenderable(Renderable p_renderable)
{
	m_Objects.push_back(p_renderable);
}


void DeferredRenderer::createSkyDome(ID3D11ShaderResourceView* p_Texture, float p_Radius)
{
	if(m_SkyDome)
		SAFE_DELETE(m_SkyDome);


	m_SkyDome = new SkyDome();
	m_SkyDome->init(m_Device, m_DeviceContext, p_Texture, p_Radius);
}


void DeferredRenderer::renderSkyDome()
{
	m_RenderSkyDome = true;
}


ID3D11ShaderResourceView* DeferredRenderer::getRT(IGraphics::RenderTarget i)
{
	switch(i)
	{
	case IGraphics::RenderTarget::DIFFUSE: return m_SRV["Diffuse"];
	case IGraphics::RenderTarget::NORMAL: return m_SRV["Normal"];
	case IGraphics::RenderTarget::W_POSITION: return m_SRV["WPosition"];
	case IGraphics::RenderTarget::SSAO: return m_SRV["SSAO"];
	case IGraphics::RenderTarget::FINAL: return m_SRV["Light"];
	default: return nullptr;
	}
}

void DeferredRenderer::updateCamera(DirectX::XMFLOAT3 p_Position)
{
	m_CameraPosition = p_Position;
}

void DeferredRenderer::updateConstantBuffer()
{
	cBuffer cb;
	cb.view = *m_ViewMatrix;
	cb.proj = *m_ProjectionMatrix;
	cb.campos = m_CameraPosition;
	m_DeviceContext->UpdateSubresource(m_Buffer["DefaultConstant"]->getBufferPointer(), NULL,NULL, &cb,NULL,NULL);
}


HRESULT DeferredRenderer::createRenderTargets(D3D11_TEXTURE2D_DESC &desc)
{
	// Create the render target texture
	HRESULT result = S_FALSE;

	//Create the render targets
	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;


	for(auto &rt : m_RT)
	{
		ID3D11Texture2D *temp;
		result = m_Device->CreateTexture2D(&desc, nullptr, &temp);
		if(FAILED(result))
			return result;


		result = m_Device->CreateRenderTargetView(temp, &rtDesc, &rt.second);
		if(FAILED(result))
			return result;


		SAFE_RELEASE(temp);
	}

	unsigned int size = m_numRenderTargets * VRAMInfo::getInstance()->calculateFormatUsage(desc.Format,
		desc.Width, desc.Height);
	VRAMInfo::getInstance()->updateUsage(size);

	return result;
}


HRESULT DeferredRenderer::createShaderResourceViews( D3D11_TEXTURE2D_DESC &desc )
{
	HRESULT result = E_FAIL;


	D3D11_SHADER_RESOURCE_VIEW_DESC dssrvdesc;
	dssrvdesc.Format = desc.Format;
	dssrvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	dssrvdesc.Texture2D.MipLevels = 1;
	dssrvdesc.Texture2D.MostDetailedMip = 0;


	ID3D11Resource* tt;


	// Make the diffuse texture from the render target.	
	m_RT[IGraphics::RenderTarget::DIFFUSE]->GetResource(&tt);
	result = m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_SRV["Diffuse"]);
	SAFE_RELEASE(tt);
	tt = nullptr;
	if(FAILED(result))
		return result;


	// Make the normal texture from the render target.
	m_RT[IGraphics::RenderTarget::NORMAL]->GetResource(&tt);
	result = m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_SRV["Normal"]);
	SAFE_RELEASE(tt);
	tt = nullptr;
	if(FAILED(result))
		return result;


	// Make the world position texture from the render target.
	m_RT[IGraphics::RenderTarget::W_POSITION]->GetResource(&tt);
	result = m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_SRV["WPosition"]);
	SAFE_RELEASE(tt);
	tt = nullptr;
	if(FAILED(result))
		return result;

	// SSAO texture
	m_RT[IGraphics::RenderTarget::SSAO]->GetResource(&tt);
	result = m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_SRV["SSAO"]);
	SAFE_RELEASE(tt);
	tt = nullptr;
	if(FAILED(result))
		return result;

	// Make the final texture from the render target.
	m_RT[IGraphics::RenderTarget::FINAL]->GetResource(&tt);
	result = m_Device->CreateShaderResourceView(tt, &dssrvdesc, &m_SRV["Light"]);
	SAFE_RELEASE(tt);
	tt = nullptr;
	if(FAILED(result))
		return result;

	return result;
}


void DeferredRenderer::createBuffers()
{
	cBuffer cb;
	cb.view = *m_ViewMatrix;
	cb.proj = *m_ProjectionMatrix;
	cb.campos = m_CameraPosition;


	Buffer::Description cbdesc;
	cbdesc.initData = &cb;
	cbdesc.numOfElements = 1;
	cbdesc.sizeOfElement = sizeof(cBuffer);
	cbdesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;
	cbdesc.usage = Buffer::Usage::DEFAULT;
	m_Buffer["DefaultConstant"] = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(cBuffer));


	cbdesc.initData = nullptr;
	cbdesc.sizeOfElement = sizeof(cObjectBuffer);
	m_Buffer["ObjectConstant"] = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(cObjectBuffer));


	cbdesc.sizeOfElement = sizeof(cAnimatedObjectBuffer);
	m_Buffer["AnimatedConstant"] = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(cAnimatedObjectBuffer));	


	Buffer::Description adesc;
	adesc.initData = nullptr;
	adesc.numOfElements = m_MaxLightsPerLightInstance;
	adesc.sizeOfElement = sizeof(Light);
	adesc.type = Buffer::Type::VERTEX_BUFFER;
	adesc.usage = Buffer::Usage::CPU_WRITE_DISCARD;
	m_Buffer["LightConstant"] = WrapperFactory::getInstance()->createBuffer(adesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(Light) * m_MaxLightsPerLightInstance);


	Buffer::Description instanceWorldDesc;
	instanceWorldDesc.initData = nullptr;
	instanceWorldDesc.numOfElements = m_MaxLightsPerLightInstance;
	instanceWorldDesc.sizeOfElement = sizeof(DirectX::XMFLOAT4X4);
	instanceWorldDesc.type = Buffer::Type::VERTEX_BUFFER;
	instanceWorldDesc.usage = Buffer::Usage::CPU_WRITE_DISCARD;
	m_Buffer["WorldInstance"] = WrapperFactory::getInstance()->createBuffer(instanceWorldDesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(DirectX::XMFLOAT4X4) * m_MaxLightsPerLightInstance);


	cbdesc.sizeOfElement = sizeof(cSSAO_Buffer);
	cbdesc.initData = nullptr;
	cbdesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;
	cbdesc.usage = Buffer::Usage::DEFAULT;

	m_Buffer["SSAOConstant"] = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(cSSAO_Buffer));

	updateSSAO_VarConstantBuffer();

	cSSAO_BlurBuffer ssaoBlurBuffer;
	ssaoBlurBuffer.horizontalBlur = true;
	ssaoBlurBuffer.texelWidth = 1.0f / m_ScreenWidth;
	ssaoBlurBuffer.texelHeight = 1.0f / m_ScreenHeight;


	cbdesc.sizeOfElement = sizeof(cSSAO_BlurBuffer);
	cbdesc.initData = &ssaoBlurBuffer;
	cbdesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;
	cbdesc.usage = Buffer::Usage::DEFAULT;
	m_Buffer["SSAOConstant_Blur"] = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(cSSAO_BlurBuffer));
}


void DeferredRenderer::buildSSAO_OffsetVectors(cSSAO_Buffer &p_Buffer)
{
	p_Buffer.offsetVectors[0] = XMFLOAT4(1, 1, 1, 0);
	p_Buffer.offsetVectors[1] = XMFLOAT4(-1, -1, -1, 0);


	p_Buffer.offsetVectors[2] = XMFLOAT4(-1, +1, +1, 0);
	p_Buffer.offsetVectors[3] = XMFLOAT4(+1, -1, -1, 0);


	p_Buffer.offsetVectors[4] = XMFLOAT4(+1, +1, -1, 0);
	p_Buffer.offsetVectors[5] = XMFLOAT4(-1, -1, +1, 0);


	p_Buffer.offsetVectors[6] = XMFLOAT4(-1, +1, -1, 0);
	p_Buffer.offsetVectors[7] = XMFLOAT4(+1, -1, +1, 0);


	p_Buffer.offsetVectors[8] = XMFLOAT4(-1, 0, 0, 0);
	p_Buffer.offsetVectors[9] = XMFLOAT4(+1, 0, 0, 0);


	p_Buffer.offsetVectors[10] = XMFLOAT4(0, -1, 0, 0);
	p_Buffer.offsetVectors[11] = XMFLOAT4(0, +1, 0, 0);


	p_Buffer.offsetVectors[12] = XMFLOAT4(0, 0, -1, 0);
	p_Buffer.offsetVectors[13] = XMFLOAT4(0, 0, +1, 0);


	std::default_random_engine randomizer;
	std::uniform_real_distribution<float> distribution(0.25f, 1.0f);

	for(int i = 0; i < 14; i++)
	{
		float s = distribution(randomizer);
		XMVECTOR v = XMLoadFloat4(&p_Buffer.offsetVectors[i]);
		v = s * XMVector4Normalize(v);
		XMStoreFloat4(&p_Buffer.offsetVectors[i], v);
	}
}




void DeferredRenderer::clearRenderTargets()
{
	float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_DeviceContext->ClearRenderTargetView(m_RT[IGraphics::RenderTarget::DIFFUSE], color);


	color[0] = color[1] = color[2] = 0.5f;
	m_DeviceContext->ClearRenderTargetView(m_RT[IGraphics::RenderTarget::NORMAL], color);


	color[0] = color[1] = color[2] = 1.0f;
	m_DeviceContext->ClearRenderTargetView(m_RT[IGraphics::RenderTarget::W_POSITION], color);

	color[0] = color[1] = color[2] = 1.0f;
	color[3] = 1.0f;
	m_DeviceContext->ClearRenderTargetView(m_RT[IGraphics::RenderTarget::SSAO], color);

	color[0] = color[1] = color[2] = color[3] = 0.0f;
	m_DeviceContext->ClearRenderTargetView(m_RT[IGraphics::RenderTarget::FINAL], color);
}


void DeferredRenderer::createSamplerState()
{
	// Create texture sampler.
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter			= D3D11_FILTER_ANISOTROPIC;
	sd.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc   = D3D11_COMPARISON_NEVER;
	sd.MinLOD			= 0;
	sd.MaxLOD			= D3D11_FLOAT32_MAX;
	m_Device->CreateSamplerState( &sd, &m_Sampler["Default"] );


	// Create SSAO random vector texture sampler.
	sd.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;//D3D11_FILTER_MIN_MAG_MIP_POINT; //Should be D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	m_Device->CreateSamplerState(&sd, &m_Sampler["SSAO_RandomVec"]);


	// Create SSAO normal depth texture sampler.
	sd.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.BorderColor[0] = sd.BorderColor[1] = sd.BorderColor[2] = 0.0f;
	sd.BorderColor[3] = 1e5f;
	m_Device->CreateSamplerState(&sd, &m_Sampler["SSAO_NormalDepth"]);


	// Create SSAO blur texture sampler
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_Device->CreateSamplerState(&sd, &m_Sampler["SSAO_Blur"]);
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


void DeferredRenderer::createShaders()
{
	ShaderInputElementDescription shaderDesc[] = 
	{
		{"POSITION",	0, Format::R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,	  0},
		{"LPOSITION",	0, Format::R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA,  1},
		{"COLOR",		0, Format::R32G32B32_FLOAT, 1, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"DIRECTION",	0, Format::R32G32B32_FLOAT, 1, 24, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"ANGLE",		0, Format::R32G32_FLOAT,	1, 36, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"RANGE",		0, Format::R32_FLOAT,		1, 44, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};


	m_Shader["SpotLight"] = WrapperFactory::getInstance()->createShader(L"assets/shaders/LightPassSpotLight.hlsl",
		"SpotLightVS,SpotLightPS", "5_0",ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER, shaderDesc, 6);


	m_Shader["PointLight"] = WrapperFactory::getInstance()->createShader(L"assets/shaders/LightPassPointLight.hlsl",
		"PointLightVS,PointLightPS", "5_0",ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER, shaderDesc, 6);


	m_Shader["DirectionalLight"] = WrapperFactory::getInstance()->createShader(L"assets/shaders/LightPassDirectionalLight.hlsl",
		"DirectionalLightVS,DirectionalLightPS", "5_0",ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER, shaderDesc, 6);


	ShaderInputElementDescription instanceshaderDesc[] = 
	{
		{"POSITION",0, Format::R32G32B32A32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",	0, Format::R32G32B32_FLOAT, 0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COORD",	0, Format::R32G32_FLOAT, 0, 28,		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",	0, Format::R32G32B32_FLOAT, 0, 36,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL",0, Format::R32G32B32_FLOAT, 0, 48,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WORLD", 0, Format::R32G32B32A32_FLOAT, 1, 0,	D3D10_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 1, Format::R32G32B32A32_FLOAT, 1, 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 2, Format::R32G32B32A32_FLOAT, 1, 32, D3D10_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 3, Format::R32G32B32A32_FLOAT, 1, 48, D3D10_INPUT_PER_INSTANCE_DATA, 1},
	};


	m_Shader["IGeometry"] = WrapperFactory::getInstance()->createShader(L"assets/shaders/GeoInstanceShader.hlsl",
		"VS,PS", "5_0",ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER, instanceshaderDesc, 9);


	m_Shader["SSAO"] = WrapperFactory::getInstance()->createShader(L"assets/shaders/SSAO.hlsl",
		"VS,PS", "5_0",ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);


	m_Shader["SSAO_Blur"] = WrapperFactory::getInstance()->createShader(L"assets/shaders/SSAO_Blur.hlsl",
		"VS,PS", "5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
}


void DeferredRenderer::loadLightModels()
{
	ModelBinaryLoader modelLoader;
	modelLoader.loadBinaryFile("assets/LightModels/SpotLight.btx");
	const std::vector<StaticVertex>& vertices = modelLoader.getStaticVertexBuffer();
	std::vector<DirectX::XMFLOAT3> temp;
	for(unsigned int i = 0; i < vertices.size(); i++)
	{
		temp.push_back(DirectX::XMFLOAT3(vertices.at(i).m_Position.x,vertices.at(i).m_Position.y,vertices.at(i).m_Position.z));
	}




	Buffer::Description cbdesc;
	cbdesc.initData = temp.data();
	cbdesc.numOfElements = temp.size();
	cbdesc.sizeOfElement = sizeof(DirectX::XMFLOAT3);
	cbdesc.type = Buffer::Type::VERTEX_BUFFER;
	cbdesc.usage = Buffer::Usage::USAGE_IMMUTABLE;
	VRAMInfo::getInstance()->updateUsage(sizeof(XMFLOAT3) * temp.size());




	m_Buffer["SpotLightModel"] = WrapperFactory::getInstance()->createBuffer(cbdesc);
	temp.clear();
	modelLoader.loadBinaryFile("assets/LightModels/Sphere2.btx");
	for(unsigned int i = 0; i < vertices.size(); i++)
	{
		temp.push_back(DirectX::XMFLOAT3(vertices.at(i).m_Position.x,vertices.at(i).m_Position.y,vertices.at(i).m_Position.z));
	}


	cbdesc.initData = temp.data();
	cbdesc.numOfElements = temp.size();
	m_Buffer["PointLightModel"] = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(XMFLOAT3) * temp.size());
	temp.clear();
	modelLoader.clear();


	//Create a quad
	temp.push_back(DirectX::XMFLOAT3(-1,1,0));
	temp.push_back(DirectX::XMFLOAT3(-1,-1,0));
	temp.push_back(DirectX::XMFLOAT3(1,1,0));
	temp.push_back(DirectX::XMFLOAT3(-1,-1,0));
	temp.push_back(DirectX::XMFLOAT3(1,-1,0));
	temp.push_back(DirectX::XMFLOAT3(1,1,0));
	cbdesc.initData = temp.data();
	cbdesc.numOfElements = 6;
	m_Buffer["DirectionalLightModel"] = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(XMFLOAT3) * temp.size());


	temp.clear();
	temp.shrink_to_fit();
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
	rdesc.CullMode = D3D11_CULL_FRONT;
	m_Device->CreateRasterizerState(&rdesc,&m_RasterState);
}


void DeferredRenderer::renderLight(Shader *p_Shader, Buffer* p_ModelBuffer, vector<Light> *p_Lights)
{
	if(p_Lights->size() > 0)
	{
		UINT Offsets[2] = {0,0};
		ID3D11Buffer * buffers[] = {p_ModelBuffer->getBufferPointer(), m_Buffer["LightConstant"]->getBufferPointer()};
		UINT Stride[2] = {sizeof(DirectX::XMFLOAT3), sizeof(Light)};


		p_Shader->setShader();
		m_DeviceContext->IASetVertexBuffers(0,2,buffers,Stride, Offsets);
		D3D11_MAPPED_SUBRESOURCE ms;
		for(unsigned int i = 0; i < p_Lights->size(); i += m_MaxLightsPerLightInstance)
		{
			int nrToCpy = (p_Lights->size() - i >= m_MaxLightsPerLightInstance) ? m_MaxLightsPerLightInstance : p_Lights->size() - i ;


			m_DeviceContext->Map(m_Buffer["LightConstant"]->getBufferPointer(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
			memcpy(ms.pData, p_Lights->data() + i, sizeof(Light) * nrToCpy);
			m_DeviceContext->Unmap(m_Buffer["LightConstant"]->getBufferPointer(), NULL);


			m_DeviceContext->DrawInstanced(p_ModelBuffer->getNumOfElements(), p_Lights->size(),0,0);
		}


		for(unsigned int i = 0; i < 2; i++)
			m_DeviceContext->IASetVertexBuffers(i,0,0,0, 0);
		p_Shader->unSetShader();
	}
}


void DeferredRenderer::createRandomTexture(unsigned int p_Size)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));


	textureDesc.Width = textureDesc.Height = p_Size;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;


	std::default_random_engine randomizer;
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	vector<DirectX::XMFLOAT3> initData;

	for(unsigned int i = 0; i < p_Size * p_Size; i++)
	{
		XMFLOAT3 randomVec;
		XMVECTOR temp = XMVector3Normalize(XMVectorSet(distribution(randomizer),
			distribution(randomizer), distribution(randomizer), 0.0f));
		XMStoreFloat3(&randomVec, temp);


		initData.push_back(randomVec);
	}


	ID3D11Texture2D *texture;


	D3D11_SUBRESOURCE_DATA subData;
	subData.SysMemPitch = sizeof(DirectX::XMFLOAT3) * p_Size;
	//subData.SysMemSlicePitch = subData.SysMemPitch * p_Size;
	subData.pSysMem = initData.data();


	m_Device->CreateTexture2D(&textureDesc, &subData, &texture);


	D3D11_SHADER_RESOURCE_VIEW_DESC dssrvdesc = {};
	dssrvdesc.Format = textureDesc.Format;
	dssrvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	dssrvdesc.Texture2D.MipLevels = 1;
	dssrvdesc.Texture2D.MostDetailedMip = 0;


	m_Device->CreateShaderResourceView(texture, &dssrvdesc, &m_SRV["SSAO_RandomVec"]);
	unsigned int size = VRAMInfo::getInstance()->calculateFormatUsage(textureDesc.Format,
		textureDesc.Width, textureDesc.Height);
	VRAMInfo::getInstance()->updateUsage(size);


	SAFE_RELEASE(texture);
}


void DeferredRenderer::renderObject(Renderable &p_Object)
{
	ID3D11ShaderResourceView *nullsrvs[] = {0,0,0};
	p_Object.model->vertexBuffer->setBuffer(0);


	if (p_Object.model->isAnimated)
	{
		cAnimatedObjectBuffer temp;
		temp.invTransposeWorld = p_Object.invTransposeWorld;


		const std::vector<DirectX::XMFLOAT4X4>* tempBones = p_Object.finalTransforms;
		for (unsigned int a = 0; a < tempBones->size(); a++)
			temp.boneTransform[a] = (*tempBones)[a];


		m_DeviceContext->UpdateSubresource(m_Buffer["AnimatedConstant"]->getBufferPointer(), NULL,NULL, &temp,NULL,NULL);
		m_Buffer["AnimatedConstant"]->setBuffer(2);
	}


	cObjectBuffer temp;
	temp.world = p_Object.world;


	m_DeviceContext->UpdateSubresource(m_Buffer["ObjectConstant"]->getBufferPointer(), NULL,NULL, &temp,NULL,NULL);
	m_Buffer["ObjectConstant"]->setBuffer(1);


	// Set shader.
	p_Object.model->shader->setShader();
	float data[] = { 1.0f, 1.0f, 1.f, 1.0f};
	p_Object.model->shader->setBlendState(m_BlendState2, data);


	for(unsigned int j = 0; j < p_Object.model->numOfMaterials;j++)
	{
		ID3D11ShaderResourceView *srvs[] =  {	p_Object.model->diffuseTexture[j].second, 
			p_Object.model->normalTexture[j].second, 
			p_Object.model->specularTexture[j].second 
		};
		m_DeviceContext->PSSetShaderResources(0, 3, srvs);


		m_DeviceContext->Draw(p_Object.model->drawInterval.at(j).second, p_Object.model->drawInterval.at(j).first);


		m_DeviceContext->PSSetShaderResources(0, 3, nullsrvs);
	}


	p_Object.model->shader->setBlendState(0, data);
	p_Object.model->shader->unSetShader();
	m_Buffer["ObjectConstant"]->unsetBuffer(1);
	m_Buffer["AnimatedConstant"]->unsetBuffer(2);
	p_Object.model->vertexBuffer->unsetBuffer(0);
}


void DeferredRenderer::SortRenderables( std::vector<Renderable> &animatedOrSingle, std::vector<std::vector<Renderable>> &instancedModels )
{
	std::sort(m_Objects.begin(),m_Objects.end(), [] (Renderable &a, Renderable &b)
	{ 
		return a.model > b.model;
	});


	for(unsigned int i = 0; i < m_Objects.size(); i++)
	{
		if(m_Objects.at(i).model->isAnimated)
			animatedOrSingle.push_back(std::move(m_Objects.at(i)));
		else
		{
			std::vector<Renderable> l;
			int current = i;
			int nr = 0;
			for(unsigned int j = i + 1; j < m_Objects.size();j++)
				if(m_Objects.at(current).model == m_Objects.at(j).model)
					nr++;
				else
					break;


			i += nr;


			if(nr >= 1)
			{
				l.reserve(nr);
				std::move(m_Objects.begin() + current, m_Objects.begin() + i + 1, std::back_inserter(l));
				instancedModels.push_back(l);
			}
			else
				animatedOrSingle.push_back(std::move(m_Objects.at(current)));
		}
	}
	m_Objects.clear();
}


void DeferredRenderer::RenderObjectsInstanced( std::vector<Renderable> &p_Objects )
{
	UINT Offsets[2] = {0,0};
	ID3D11Buffer * buffers[] = {p_Objects.front().model->vertexBuffer->getBufferPointer(), m_Buffer["WorldInstance"]->getBufferPointer()};
	UINT Stride[2] = {60, sizeof(DirectX::XMFLOAT4X4)};


	ID3D11ShaderResourceView *nullsrvs[] = {0,0,0};


	// Set shader.
	m_Shader["IGeometry"]->setShader();
	float data[] = { 1.0f, 1.0f, 1.f, 1.0f};
	m_Shader["IGeometry"]->setBlendState(m_BlendState2, data);
	m_DeviceContext->IASetVertexBuffers(0,2,buffers,Stride, Offsets);


	for(unsigned int u = 0; u < p_Objects.front().model->numOfMaterials;u++)
	{
		ID3D11ShaderResourceView *srvs[] =  {	p_Objects.front().model->diffuseTexture[u].second, 
			p_Objects.front().model->normalTexture[u].second, 
			p_Objects.front().model->specularTexture[u].second 
		};
		m_DeviceContext->PSSetShaderResources(0, 3, srvs);
		D3D11_MAPPED_SUBRESOURCE ms;
		for(unsigned int i = 0; i < p_Objects.size(); i += m_MaxLightsPerLightInstance)
		{
			int nrToCpy = (p_Objects.size() - i >= m_MaxLightsPerLightInstance) ? m_MaxLightsPerLightInstance : p_Objects.size() - i ;
			std::vector<XMFLOAT4X4> tWorld;
			tWorld.reserve(nrToCpy);
			for(int j = 0; j < nrToCpy; j++)
				tWorld.push_back(p_Objects.at(i+j).world);


			m_DeviceContext->Map(m_Buffer["WorldInstance"]->getBufferPointer(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
			memcpy(ms.pData, tWorld.data(), sizeof(DirectX::XMFLOAT4X4) * tWorld.size());
			m_DeviceContext->Unmap(m_Buffer["WorldInstance"]->getBufferPointer(), NULL);


			m_DeviceContext->DrawInstanced(p_Objects.front().model->drawInterval.at(u).second, tWorld.size(),
				p_Objects.front().model->drawInterval.at(u).first,0);
		}
		m_DeviceContext->PSSetShaderResources(0, 3, nullsrvs);
	}


	for(unsigned int i = 0; i < 2; i++)
		m_DeviceContext->IASetVertexBuffers(i,0,0,0, 0);
	m_Shader["IGeometry"]->setBlendState(0, data);
	m_Shader["IGeometry"]->unSetShader();
}