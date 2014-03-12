#include "ScreenRenderer.h"
#include "ConstantBuffers.h"
#include "WrapperFactory.h"
#include "VRAMInfo.h"
#include "Utilities/MemoryUtil.h"
#include "GraphicsExceptions.h"
#include <algorithm>
#include <functional>

using namespace DirectX;

ScreenRenderer::ScreenRenderer(void)
{
	m_DeviceContext = nullptr;

	m_DepthStencilView = nullptr;
	m_RenderTarget = nullptr;
	m_Sampler = nullptr;
	m_RasterState = nullptr;
	m_DepthStencilState = nullptr;

	m_ViewMatrix = nullptr;

	m_ConstantBuffer = nullptr;
	m_ObjectConstantBuffer = nullptr;
	m_HUD_Shader = nullptr;
	m_TransparencyAdditiveBlend = nullptr;
}


ScreenRenderer::~ScreenRenderer(void)
{
	m_DeviceContext = nullptr;

	m_DepthStencilView = nullptr;
	m_RenderTarget = nullptr;
	SAFE_RELEASE(m_Sampler);
	SAFE_RELEASE(m_RasterState);
	SAFE_RELEASE(m_DepthStencilState);
	
	m_ViewMatrix = nullptr;

	SAFE_DELETE(m_ConstantBuffer);
	SAFE_DELETE(m_ObjectConstantBuffer);
	SAFE_DELETE(m_HUD_Shader);
	SAFE_RELEASE(m_TransparencyAdditiveBlend);
}

void ScreenRenderer::initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext,
	XMFLOAT4X4 *p_ViewMatrix, XMFLOAT4 p_OrthoData, ID3D11DepthStencilView* p_DepthStencilView,
	ID3D11RenderTargetView *p_RenderTarget)
{
	D3D11_BLEND_DESC blendDesc = createBlendStateDescription();
	D3D11_SAMPLER_DESC samplerDesc = createSamplerDescription();
	D3D11_RASTERIZER_DESC rasterDesc =  createRasterStateDescription();
	D3D11_DEPTH_STENCIL_DESC depthDesc = createDepthStencilStateDescription();
	if(!p_Device || !p_DeviceContext)
		throw GraphicsException("Failed to initialize screen renderer, null devices not allowed", __LINE__, __FILE__);
	
	m_DeviceContext = p_DeviceContext;
	m_DepthStencilView = p_DepthStencilView;
	m_RenderTarget = p_RenderTarget;

	m_ViewMatrix = p_ViewMatrix;
	XMStoreFloat4x4(&m_OrthoMatrix,
		XMMatrixTranspose(XMMatrixOrthographicLH(p_OrthoData.x, p_OrthoData.y, p_OrthoData.z, p_OrthoData.w)));
	
	m_HUD_Shader = WrapperFactory::getInstance()->createShader(L"assets/shaders/HUD_Shader.hlsl", "VS,PS", "5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);

	p_Device->CreateBlendState(&blendDesc, &m_TransparencyAdditiveBlend);
	p_Device->CreateSamplerState(&samplerDesc, &m_Sampler);
	p_Device->CreateRasterizerState(&rasterDesc, &m_RasterState);
	p_Device->CreateDepthStencilState(&depthDesc, &m_DepthStencilState);
	createBuffers();
}

void ScreenRenderer::add2D_Object(Renderable2D &p_Object)
{
	m_2D_Objects.push_back(p_Object);
}

D3D11_BLEND_DESC ScreenRenderer::createBlendStateDescription(void)
{
	D3D11_BLEND_DESC bd;
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend =  D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return bd;
}

void ScreenRenderer::createBuffers(void)
{
	c2D_ObjectBuffer cb;
	cb.view = *m_ViewMatrix;
	cb.orthoProj = m_OrthoMatrix;

	Buffer::Description cbdesc;
	cbdesc.initData = &cb;
	cbdesc.numOfElements = 1;
	cbdesc.sizeOfElement = sizeof(c2D_ObjectBuffer);
	cbdesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;
	cbdesc.usage = Buffer::Usage::DEFAULT;

	m_ConstantBuffer = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(c2D_ObjectBuffer));

	cbdesc.initData = nullptr;
	cbdesc.sizeOfElement = sizeof(cObjectBufferColor);
	m_ObjectConstantBuffer = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(cObjectBufferColor));
}

D3D11_SAMPLER_DESC ScreenRenderer::createSamplerDescription(void)
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

	return sd;
}

D3D11_RASTERIZER_DESC ScreenRenderer::createRasterStateDescription(void)
{
	D3D11_RASTERIZER_DESC rd;

	//Setup the raster description which will determine how and what polygons will be drawn.
	rd.AntialiasedLineEnable = false;
	rd.CullMode = D3D11_CULL_NONE;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = false;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.FrontCounterClockwise = false;
	rd.MultisampleEnable = false;
	rd.ScissorEnable = false;
	rd.SlopeScaledDepthBias = 0.0f;

	return rd;
}

D3D11_DEPTH_STENCIL_DESC ScreenRenderer::createDepthStencilStateDescription(void)
{
	D3D11_DEPTH_STENCIL_DESC sd = {0};
	sd.DepthEnable = true;
	sd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	sd.DepthFunc = D3D11_COMPARISON_LESS;

	sd.StencilEnable = false;
	sd.StencilReadMask = 0xFF;
	sd.StencilWriteMask = 0xFF;

	//Stencil operations if pixel is front-facing.
	sd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	sd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	sd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	sd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	sd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	sd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	sd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	sd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	return sd;
}

void ScreenRenderer::updateConstantBuffer(void)
{
	c2D_ObjectBuffer cb;
	cb.view = *m_ViewMatrix;
	cb.orthoProj = m_OrthoMatrix;
	m_DeviceContext->UpdateSubresource(m_ConstantBuffer->getBufferPointer(), NULL, nullptr, &cb, NULL, NULL);
}

void ScreenRenderer::renderScreen(void)
{
	if(m_2D_Objects.size() > 0)
	{
		m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

		// Store previous States to be set when we exit the method.
		ID3D11RasterizerState *previousRasterState;
		ID3D11DepthStencilState *previousDepthState;
		m_DeviceContext->RSGetState(&previousRasterState);
		m_DeviceContext->OMGetDepthStencilState(&previousDepthState,0);
		
		m_DeviceContext->RSSetState(m_RasterState);
		m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState,0);

		//Sort objects by the range to the camera
		std::sort(m_2D_Objects.begin(), m_2D_Objects.end(),
			[] (Renderable2D &a, Renderable2D &b){ return a.position.z > b.position.z; });

		// Set the render targets.
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTarget, m_DepthStencilView);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_ConstantBuffer->setBuffer(0);
		m_ObjectConstantBuffer->setBuffer(1);
		updateConstantBuffer();

		for(auto &object : m_2D_Objects)
		{
			cObjectBufferColor temp;
			temp.world = object.getWorldMatrix();
			temp.color = object.color;

			m_DeviceContext->UpdateSubresource(m_ObjectConstantBuffer->getBufferPointer(), NULL, nullptr,
				&temp, NULL, NULL);

			renderObject(object);
		}

		m_ObjectConstantBuffer->unsetBuffer(1);
		m_ConstantBuffer->unsetBuffer(0);

		// Unset render targets.
		m_DeviceContext->OMSetRenderTargets(0, 0, 0);
		m_DeviceContext->RSSetState(previousRasterState);
		m_DeviceContext->OMSetDepthStencilState(previousDepthState,0);

		SAFE_RELEASE(previousRasterState);
		SAFE_RELEASE(previousDepthState);
		m_2D_Objects.clear();
	}
}

void ScreenRenderer::renderObject(Renderable2D &p_Object)
{
	m_DeviceContext->PSSetSamplers(0, 1, &m_Sampler);
	
	p_Object.model->vertexBuffer->setBuffer(0);

	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set shader.
	m_HUD_Shader->setShader();
	float data[] = { 1.0f, 1.0f, 1.f, 1.0f};
	m_HUD_Shader->setBlendState(m_TransparencyAdditiveBlend, data);
	m_DeviceContext->PSSetShaderResources(0, 1, &(p_Object.model->diffuseTexture[0].second));

	const auto& material = p_Object.model->materialSets.at(0).second.at(0);
	m_DeviceContext->Draw(material.numOfVertices, material.vertexStart);

	ID3D11ShaderResourceView *nullSrv = 0;
	m_DeviceContext->PSSetShaderResources(0, 1, &nullSrv);
	data[0] = data[1] = data[2] = data[3] = 0.0f; 
	m_HUD_Shader->setBlendState(0, data);
	m_HUD_Shader->unSetShader();
	p_Object.model->vertexBuffer->unsetBuffer(0);
	m_DeviceContext->PSSetSamplers(0,0,0);
}