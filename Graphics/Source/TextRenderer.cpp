#include "TextRenderer.h"
#include "VRAMInfo.h"
#include "Utilities/MemoryUtil.h"
#include "GraphicsExceptions.h"

#include <functional>
#include <algorithm>
#include <string>

using namespace DirectX;

TextRenderer::TextRenderer(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_CameraPosition = nullptr;
	m_ViewMatrix = nullptr;
	m_ProjectionMatrix = nullptr;
	m_RenderTargetView = nullptr;
	m_Sampler = nullptr;
	m_RasterState = nullptr;
}

TextRenderer::~TextRenderer(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_CameraPosition = nullptr;
	m_ViewMatrix = nullptr;
	m_ProjectionMatrix = nullptr;
	m_RenderTargetView = nullptr;

	SAFE_RELEASE(m_Sampler);
	SAFE_RELEASE(m_RasterState);
}

void TextRenderer::initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, XMFLOAT3 *p_CameraPosition,
	XMFLOAT4X4 *p_ViewMatrix, XMFLOAT4X4 *p_ProjectionMatrix, ID3D11RenderTargetView *p_RenderTarget)
{
	if(!p_Device || !p_DeviceContext)
		throw TextRendererException("Failed to initialize text renderer, nullpointers are not allowed. ",
		__LINE__, __FILE__);
	m_Device = p_Device;
	m_DeviceContext = p_DeviceContext;
	m_CameraPosition = p_CameraPosition;
	m_ViewMatrix = p_ViewMatrix;
	m_ProjectionMatrix = p_ProjectionMatrix;
	m_RenderTargetView = p_RenderTarget;

	createBuffers();

	m_Shader = WrapperFactory::getInstance()->createShader(L"assets/shaders/BillboardShader.hlsl", "VS,PS,GS", "5_0",
		ShaderType::VERTEX_SHADER | ShaderType::GEOMETRY_SHADER | ShaderType::PIXEL_SHADER);

	createBlendState();
	createSamplerState();
	createRasterizerState();
}

void TextRenderer::addTextObject(TextId p_InstanceId, TextInstance &p_Object)
{
	if(m_TextList.count(p_InstanceId) > 0)		
		throw TextRendererException("Failed to add text object, ID already exists: " + std::to_string(p_InstanceId),
			__LINE__, __FILE__);

	m_TextList.insert(std::pair<TextId, TextInstance>(p_InstanceId, p_Object));
}

void TextRenderer::removeTextObject(TextId p_InstanceId)
{
	if(m_TextList.count(p_InstanceId) > 0)
		m_TextList.erase(p_InstanceId);
	else
		throw TextRendererException("Failed to remove text object, ID does not exist: " + std::to_string(p_InstanceId),
			__LINE__, __FILE__);
}

void TextRenderer::renderTextObject(TextId p_InstanceId)
{
	if(m_TextList.count(p_InstanceId) > 0)
		m_RenderList.push_back(m_TextList.at(p_InstanceId));
	else
		throw;
}

void TextRenderer::renderFrame(void)
{
	if(m_RenderList.size() > 0)
	{
		ID3D11RasterizerState *previousRasterState;
		ID3D11DepthStencilState *previousDepthState;
		m_DeviceContext->RSGetState(&previousRasterState);
		m_DeviceContext->OMGetDepthStencilState(&previousDepthState,0);

		m_DeviceContext->RSSetState(m_RasterState);
		//m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState,0);

		//Sort objects by the range to the camera
		std::sort(m_RenderList.begin(),m_RenderList.end(),std::bind(&TextRenderer::depthSortCompareFunc, this, std::placeholders::_1, std::placeholders::_2));

		// Set the render targets.
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_DeviceContext->PSSetSamplers(0, 1, &m_Sampler);

		updateConstantBuffers();

		m_Shader->setShader();
		m_Buffer->setBuffer(0);
		float data[] = { 1.0f, 1.0f, 1.f, 1.0f};
		m_Shader->setBlendState(m_TransparencyAdditiveBlend, data);
		for(auto& object : m_RenderList)
		{
			m_DeviceContext->UpdateSubresource(m_Buffer->getBufferPointer(), 0, nullptr, &object.data, 0, 0);
			m_DeviceContext->PSSetShaderResources(0, 1, &object.srv);

			m_DeviceContext->Draw(1,0);
		}
		m_Shader->setBlendState(0, data);
		m_Buffer->unsetBuffer(0);
		m_Shader->unSetShader();
		m_ConstantBuffer->unsetBuffer(0);
		// Unset render targets.
		ID3D11ShaderResourceView *nullsrv[] = {NULL};
		m_DeviceContext->PSSetShaderResources(0, 1, nullsrv);

		m_DeviceContext->OMSetRenderTargets(0, 0, 0);
		m_DeviceContext->RSSetState(previousRasterState);
		m_DeviceContext->OMSetDepthStencilState(previousDepthState,0);
		float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
		UINT sampleMask = 0xffffffff;
		m_DeviceContext->OMSetBlendState(0, blendFactor, sampleMask);
		SAFE_RELEASE(previousRasterState);
		SAFE_RELEASE(previousDepthState);

		m_RenderList.clear();
	}
}

void TextRenderer::setPosition(TextId p_InstanceId, Vector3 p_Position)
{
	if(m_TextList.count(p_InstanceId) > 0)
		m_TextList.at(p_InstanceId).data.position = p_Position;
	else
		throw;
}
void TextRenderer::setScale(TextId p_Instance, float p_Scale)
{
	if(m_TextList.count(p_Instance) > 0)
		m_TextList.at(p_Instance).data.scale = p_Scale;
	else
		throw;
}

void TextRenderer::setRotation(TextId p_Instance, float p_Rotation)
{
	if(m_TextList.count(p_Instance) > 0)
		m_TextList.at(p_Instance).data.rotation = p_Rotation;
	else
		throw;
}

void TextRenderer::createBuffers(void)
{
	cBuffer cb;
	cb.campos = *m_CameraPosition;
	cb.view = *m_ViewMatrix;
	cb.proj = *m_ProjectionMatrix;
	Buffer::Description bufferDesc;
	bufferDesc.initData = &cb;
	bufferDesc.numOfElements = 1;
	bufferDesc.sizeOfElement = sizeof(cBuffer);
	bufferDesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;
	bufferDesc.usage = Buffer::Usage::DEFAULT;
	m_ConstantBuffer = WrapperFactory::getInstance()->createBuffer(bufferDesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(cBuffer));

	bufferDesc.initData = nullptr;
	bufferDesc.sizeOfElement = sizeof(TextInstanceShaderData);
	bufferDesc.type = Buffer::Type::VERTEX_BUFFER;
	m_Buffer = WrapperFactory::getInstance()->createBuffer(bufferDesc);
}

void TextRenderer::createSamplerState(void)
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
	m_Device->CreateSamplerState( &sd, &m_Sampler );
}

void TextRenderer::createBlendState(void)
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
	m_Device->CreateBlendState(&bd, &m_TransparencyAdditiveBlend);
}

void TextRenderer::createRasterizerState(void)
{
	D3D11_RASTERIZER_DESC rd;
	rd.AntialiasedLineEnable = false;
	rd.CullMode = D3D11_CULL_NONE;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = true;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.FrontCounterClockwise = false;
	rd.MultisampleEnable = false;
	rd.ScissorEnable = false;
	rd.SlopeScaledDepthBias = 0.0f;
	m_Device->CreateRasterizerState(&rd, &m_RasterState);
}

void TextRenderer::updateConstantBuffers(void)
{
	cBuffer cb;
	cb.campos = *m_CameraPosition;
	cb.view = *m_ViewMatrix;
	cb.proj = *m_ProjectionMatrix;

	m_DeviceContext->UpdateSubresource(m_ConstantBuffer->getBufferPointer(), 0,nullptr, &cb, 0, 0);
	m_ConstantBuffer->setBuffer(0);
}

bool TextRenderer::depthSortCompareFunc(const TextInstance &a, const TextInstance &b)
{
	XMVECTOR aV = Vector3ToXMVECTOR(&a.data.position, 1.0f);
	XMVECTOR bV = Vector3ToXMVECTOR(&b.data.position, 1.0f);
	XMVECTOR eV = XMLoadFloat3(m_CameraPosition);

	XMVECTOR aVeVLength = XMVector3Length(aV - eV);
	XMVECTOR bVeVLength = XMVector3Length(bV - eV);

	return aVeVLength.m128_f32[0] > bVeVLength.m128_f32[0];
}