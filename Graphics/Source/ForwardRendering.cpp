#include "ForwardRendering.h"
#include <algorithm>
#include <functional>

ForwardRendering::ForwardRendering(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;

	m_DepthStencilView = nullptr;
	m_RenderTarget = nullptr;
	m_Sampler = nullptr;
	m_RasterState = nullptr;

	m_CameraPosition = nullptr;
	m_ViewMatrix = nullptr;
	m_ProjectionMatrix = nullptr;

	m_ConstantBuffer = nullptr;
	m_ObjectConstantBuffer = nullptr;
	m_AnimatedObjectConstantBuffer = nullptr;
	m_TransparencyAdditiveBlend = nullptr;
}


ForwardRendering::~ForwardRendering(void)
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;

	m_DepthStencilView = nullptr;
	m_RenderTarget = nullptr;
	SAFE_RELEASE(m_Sampler);
	SAFE_RELEASE(m_RasterState);
	
	m_CameraPosition = nullptr;
	m_ViewMatrix = nullptr;
	m_ProjectionMatrix = nullptr;

	SAFE_DELETE(m_ConstantBuffer);
	SAFE_DELETE(m_ObjectConstantBuffer);
	SAFE_DELETE(m_AnimatedObjectConstantBuffer);
	SAFE_RELEASE(m_TransparencyAdditiveBlend);
}

void ForwardRendering::init(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext,
							DirectX::XMFLOAT3 *p_CameraPosition, DirectX::XMFLOAT4X4 *p_ViewMatrix,
							DirectX::XMFLOAT4X4 *p_ProjectionMatrix, ID3D11DepthStencilView* p_DepthStencilView,
							ID3D11RenderTargetView *p_RenderTarget)
{
	m_Device = p_Device;
	m_DeviceContext = p_DeviceContext;

	m_DepthStencilView = p_DepthStencilView;
	m_RenderTarget = p_RenderTarget;

	m_CameraPosition = p_CameraPosition;
	m_ViewMatrix = p_ViewMatrix;
	m_ProjectionMatrix = p_ProjectionMatrix;

	createBlendStates();
	createForwardBuffers();
	createSampler();
	createRasterState();
}

void ForwardRendering::addRenderable(DeferredRenderer::Renderable p_Renderable)
{
	m_TransparencyObjects.push_back(p_Renderable);
}

void ForwardRendering::createBlendStates()
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

void ForwardRendering::createForwardBuffers()
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
	VRAMInfo::getInstance()->updateUsage(sizeof(cBuffer));

	cbdesc.initData = nullptr;
	cbdesc.sizeOfElement = sizeof(cObjectBuffer);
	m_ObjectConstantBuffer = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(cObjectBuffer));

	cbdesc.sizeOfElement = sizeof(cAnimatedObjectBuffer);
	m_AnimatedObjectConstantBuffer = WrapperFactory::getInstance()->createBuffer(cbdesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(cAnimatedObjectBuffer));
}
void ForwardRendering::createSampler()
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	sd.MinLOD			= 0;
	sd.MaxLOD           = D3D11_FLOAT32_MAX;

	m_Sampler = nullptr;
	m_Device->CreateSamplerState( &sd, &m_Sampler );
}

void ForwardRendering::createRasterState()
{
	D3D11_RASTERIZER_DESC rasterDesc;

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

	m_Device->CreateRasterizerState(&rasterDesc, &m_RasterState);
}

void ForwardRendering::updateConstantBuffer()
{
	cBuffer cb;
	cb.view = *m_ViewMatrix;
	cb.proj = *m_ProjectionMatrix;
	cb.campos = *m_CameraPosition;
	m_DeviceContext->UpdateSubresource(m_ConstantBuffer->getBufferPointer(), NULL,NULL, &cb,NULL,NULL);
}

void ForwardRendering::renderForward()
{
	if(m_TransparencyObjects.size() > 0)
	{
		// Store previous States to be set when we exit the method.
		ID3D11RasterizerState *previousRasterState;
		ID3D11DepthStencilState *previousDepthState;
		m_DeviceContext->RSGetState(&previousRasterState);
		m_DeviceContext->OMGetDepthStencilState(&previousDepthState,0);
		
		m_DeviceContext->RSSetState(m_RasterState);

		//Sort objects by the range to the camera
		std::sort(m_TransparencyObjects.begin(),m_TransparencyObjects.end(),std::bind(&ForwardRendering::depthSortCompareFunc, this, std::placeholders::_1, std::placeholders::_2));

		// Set the render targets.
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTarget, m_DepthStencilView);
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// The textures will be needed to be grabbed from the model later.
		ID3D11ShaderResourceView *nullsrvs[] = {0,0,0};

		m_ConstantBuffer->setBuffer(1);
		m_DeviceContext->PSSetSamplers(0,1,&m_Sampler);
		updateConstantBuffer();
		for(unsigned int i = 0; i < m_TransparencyObjects.size();i++)
		{
			m_TransparencyObjects.at(i).m_Model->vertexBuffer->setBuffer(0);

			if (m_TransparencyObjects.at(i).m_Model->m_IsAnimated)
			{
				cAnimatedObjectBuffer temp;
				temp.invTransposeWorld = m_TransparencyObjects.at(i).m_invTransposeWorld;

				const std::vector<DirectX::XMFLOAT4X4>* tempBones = m_TransparencyObjects.at(i).m_FinalTransforms;
				for (unsigned int a = 0; a < tempBones->size(); a++)
					temp.boneTransform[a] = (*tempBones)[a];

				m_DeviceContext->UpdateSubresource(m_AnimatedObjectConstantBuffer->getBufferPointer(), NULL,NULL, &temp,NULL,NULL);
				m_AnimatedObjectConstantBuffer->setBuffer(3);
			}

			cObjectBuffer temp;
			temp.world = m_TransparencyObjects.at(i).m_World;
			m_DeviceContext->UpdateSubresource(m_ObjectConstantBuffer->getBufferPointer(), NULL,NULL, &temp,NULL,NULL);
			m_ObjectConstantBuffer->setBuffer(2);

			// Set shader.
			m_TransparencyObjects.at(i).m_Model->shader->setShader();
			float data[] = { 1.0f, 1.0f, 1.f, 1.0f};
			m_TransparencyObjects.at(i).m_Model->shader->setBlendState(m_TransparencyAdditiveBlend, data);

			for(unsigned int j = 0; j < m_TransparencyObjects.at(i).m_Model->numOfMaterials;j++)
			{
				ID3D11ShaderResourceView *srvs[] =  {	m_TransparencyObjects.at(i).m_Model->diffuseTexture[j].second, 
					m_TransparencyObjects.at(i).m_Model->normalTexture[j].second, 
					m_TransparencyObjects.at(i).m_Model->specularTexture[j].second 
				};
				m_DeviceContext->PSSetShaderResources(0, 3, srvs);

				m_DeviceContext->Draw(m_TransparencyObjects.at(i).m_Model->drawInterval.at(j).second, m_TransparencyObjects.at(i).m_Model->drawInterval.at(j).first);

				m_DeviceContext->PSSetShaderResources(0, 3, nullsrvs);
			}

			m_TransparencyObjects.at(i).m_Model->shader->setBlendState(0, data);
			m_TransparencyObjects.at(i).m_Model->shader->unSetShader();
			m_ObjectConstantBuffer->unsetBuffer(2);
			m_AnimatedObjectConstantBuffer->unsetBuffer(3);
			m_TransparencyObjects.at(i).m_Model->vertexBuffer->unsetBuffer(0);
		}
		m_DeviceContext->PSSetSamplers(0,0,0);
		m_ConstantBuffer->unsetBuffer(1);

		// Unset render targets.
		m_DeviceContext->OMSetRenderTargets(0, 0, 0);
		m_DeviceContext->RSSetState(previousRasterState);
		m_DeviceContext->OMSetDepthStencilState(previousDepthState,0);
		float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
		UINT sampleMask = 0xffffffff;
		m_DeviceContext->OMSetBlendState(0, blendFactor, sampleMask);
		SAFE_RELEASE(previousRasterState);
		SAFE_RELEASE(previousDepthState);
		m_TransparencyObjects.clear();
	}
}

bool ForwardRendering::depthSortCompareFunc(const DeferredRenderer::Renderable &a, const DeferredRenderer::Renderable &b)
{
	DirectX::XMFLOAT3 aa = DirectX::XMFLOAT3(a.m_World._14,a.m_World._24,a.m_World._34);
	DirectX::XMFLOAT3 bb = DirectX::XMFLOAT3(b.m_World._14,b.m_World._24,b.m_World._34);

	DirectX::XMVECTOR aV = DirectX::XMLoadFloat3(&aa);
	DirectX::XMVECTOR bV = DirectX::XMLoadFloat3(&bb);
	DirectX::XMVECTOR eV = DirectX::XMLoadFloat3(m_CameraPosition);
	
	using DirectX::operator -;
	DirectX::XMVECTOR aVeVLength = DirectX::XMVector3Length(aV - eV);
	DirectX::XMVECTOR bVeVLength = DirectX::XMVector3Length(bV - eV);

	return aVeVLength.m128_f32[0] > bVeVLength.m128_f32[0];
}