#include "SkyDome.h"
#include "Utilities/MemoryUtil.h"
#include "VRAMInfo.h"

using namespace DirectX;

SkyDome::SkyDome()
{
	m_DeviceContext = nullptr;
	m_SkyDomeBuffer = nullptr;
	m_SkyDomeShader = nullptr;
	m_SkyDomeSRV = nullptr;
	m_SkyDomeDepthStencilState = nullptr;
	m_SkyDomeRasterizerState = nullptr;
	m_SkyDomeSampler = nullptr;
}

SkyDome::~SkyDome()
{
	m_DeviceContext = nullptr;

	SAFE_DELETE(m_SkyDomeBuffer);
	SAFE_DELETE(m_SkyDomeShader);
	SAFE_RELEASE(m_SkyDomeSRV);
	SAFE_RELEASE(m_SkyDomeDepthStencilState);
	SAFE_RELEASE(m_SkyDomeRasterizerState);
	SAFE_RELEASE(m_SkyDomeSampler);
}

bool SkyDome::init(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, 
				   ID3D11ShaderResourceView* p_Texture, float p_Radius)
{
	m_DeviceContext = p_DeviceContext;
	
	ID3D11Resource *resource;
	ID3D11Texture2D *texture;
	D3D11_TEXTURE2D_DESC textureDesc;

	p_Texture->GetResource(&resource);
	resource->QueryInterface(&texture);
	texture->GetDesc(&textureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = textureDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	viewDesc.TextureCube.MipLevels = textureDesc.MipLevels;
	viewDesc.TextureCube.MostDetailedMip = 0;

	p_Device->CreateShaderResourceView(texture, &viewDesc, &m_SkyDomeSRV);
	SAFE_RELEASE(texture);
	SAFE_RELEASE(resource);


	std::vector<DirectX::XMFLOAT3> temp = buildGeoSphere(1, p_Radius);
	Buffer::Description cbdesc;
	cbdesc.initData = temp.data();
	cbdesc.numOfElements = temp.size();
	cbdesc.sizeOfElement = sizeof(DirectX::XMFLOAT3);
	cbdesc.type = Buffer::Type::VERTEX_BUFFER;
	cbdesc.usage = Buffer::Usage::USAGE_IMMUTABLE;
	m_SkyDomeBuffer = WrapperFactory::getInstance()->createBuffer(cbdesc);

	VRAMInfo::getInstance()->updateUsage(temp.size() * sizeof(DirectX::XMFLOAT3));

	D3D11_DEPTH_STENCIL_DESC dsdesc;
	ZeroMemory( &dsdesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	dsdesc.DepthEnable = true;
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	p_Device->CreateDepthStencilState(&dsdesc, &m_SkyDomeDepthStencilState);

	D3D11_RASTERIZER_DESC rdesc;
	ZeroMemory( &rdesc, sizeof( D3D11_RASTERIZER_DESC ) );
	rdesc.FillMode = D3D11_FILL_SOLID;
	rdesc.CullMode = D3D11_CULL_NONE;
	p_Device->CreateRasterizerState(&rdesc,&m_SkyDomeRasterizerState);

	m_SkyDomeShader = WrapperFactory::getInstance()->createShader(L"assets/shaders/SkyDome.hlsl",
		"VS,PS","5_0",ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);

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
	p_Device->CreateSamplerState( &sd, &m_SkyDomeSampler );

	return true;
}

void SkyDome::RenderSkyDome(ID3D11RenderTargetView *p_RenderTarget, ID3D11DepthStencilView *p_DepthStencilView,
							Buffer *p_ConstantBuffer)
{
	////Select the third render target[3]
	m_DeviceContext->OMSetRenderTargets(1, &p_RenderTarget, p_DepthStencilView); 
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->RSSetState(m_SkyDomeRasterizerState);
	m_DeviceContext->OMSetDepthStencilState(m_SkyDomeDepthStencilState,0);
	m_DeviceContext->PSSetSamplers(0,1,&m_SkyDomeSampler);
	m_DeviceContext->PSSetShaderResources(0,1,&m_SkyDomeSRV);
	//Set constant data
	p_ConstantBuffer->setBuffer(0);

	m_SkyDomeShader->setShader();
	m_SkyDomeBuffer->setBuffer(0);

	m_DeviceContext->Draw(m_SkyDomeBuffer->getNumOfElements(),0);

	m_SkyDomeShader->unSetShader();
	m_SkyDomeBuffer->unsetBuffer(0);
	m_SkyDomeShader->unSetShader();
	p_ConstantBuffer->unsetBuffer(0);
	m_DeviceContext->PSSetSamplers(0,0,0);

	ID3D11ShaderResourceView * nullsrv[] = {0};
	m_DeviceContext->PSSetShaderResources(0,1,nullsrv);
}

std::vector<DirectX::XMFLOAT3> SkyDome::buildGeoSphere(unsigned int p_NumSubDivisions, float p_Radius)
{
	std::vector<DirectX::XMFLOAT3> Vertices;
	std::vector<unsigned int> InitIndices;

	//Put a cap on the number of subdivisions
	p_NumSubDivisions = min(p_NumSubDivisions, unsigned int(5));

	//Approximate a sphere by tesselating an icosahedron
	const float X = 0.525731f;
	const float Z = 0.850651f;

	XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-X, 0.0f, Z), XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X),  XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f),  XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
	};

	unsigned int k[60] =
	{
		1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
		1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
	};

	Vertices.resize(12);
	InitIndices.resize(60);

	for(unsigned int i = 0; i < Vertices.capacity(); i++)
	{
		Vertices[i] = pos[i];
	}
	for(unsigned int i = 0; i < InitIndices.capacity(); i++)
	{
		InitIndices[i] = k[i];
	}
	for(unsigned int i = 0; i < p_NumSubDivisions; i++)
	{
		subdivide(Vertices, InitIndices);        
	}

	// Project m_vertices onto sphere and scale.
	for(size_t i = 0; i < Vertices.size(); ++i)
	{
		XMVECTOR tv = XMLoadFloat3(&Vertices[i]);
		tv = XMVector3Normalize(tv);
		XMStoreFloat3(&Vertices[i],tv);
		Vertices[i].x *= p_Radius;
		Vertices[i].y *= p_Radius;
		Vertices[i].z *= p_Radius;
	}



	std::vector<XMFLOAT3> initSkym_vertices(Vertices.size());
	for(unsigned int i = 0; i < Vertices.size(); i++)
	{
		initSkym_vertices[i].x = 0.5f * Vertices[i].x;
		initSkym_vertices[i].y = 0.5f * Vertices[i].y;
		initSkym_vertices[i].z = 0.5f * Vertices[i].z;
	}

	std::vector<XMFLOAT3> temp;

	for(unsigned int i = 0; i < InitIndices.size();i++)
	{
		temp.push_back(Vertices.at(InitIndices.at(i)));
	}
	Vertices.clear();
	Vertices.shrink_to_fit();
	InitIndices.clear();
	InitIndices.shrink_to_fit();

	return temp;
}

void SkyDome::subdivide(std::vector<DirectX::XMFLOAT3> &p_Vertices, std::vector<unsigned int> &p_InitIndices)
{
	std::vector<XMFLOAT3> vin = p_Vertices;
	std::vector<unsigned int> iin = p_InitIndices;

	p_Vertices.resize(0);
	p_InitIndices.resize(0);

	// v1
	// *
	// / \
	// / \
	// m0*-----*m1
	// / \ / \
	// / \ / \
	// *-----*-----*
	// v0 m2 v2
	unsigned int numTris = (unsigned int)iin.size()/3;
	for(unsigned int i = 0; i < numTris; ++i)
	{
		XMFLOAT3 v0 = vin[ iin[i*3+0] ];
		XMFLOAT3 v1 = vin[ iin[i*3+1] ];
		XMFLOAT3 v2 = vin[ iin[i*3+2] ];

		XMVECTOR vv0 = XMLoadFloat3(&v0);
		XMVECTOR vv1 = XMLoadFloat3(&v1);
		XMVECTOR vv2 = XMLoadFloat3(&v2);

		XMFLOAT3 m0;
		XMStoreFloat3(&m0,0.5f*(vv0 + vv1));
		XMFLOAT3 m1;
		XMStoreFloat3(&m1,0.5f*(vv1 + vv2));
		XMFLOAT3 m2;
		XMStoreFloat3(&m2,0.5f*(vv0 + vv2));

		p_Vertices.push_back(v0); // 0
		p_Vertices.push_back(v1); // 1
		p_Vertices.push_back(v2); // 2
		p_Vertices.push_back(m0); // 3
		p_Vertices.push_back(m1); // 4
		p_Vertices.push_back(m2); // 5

		p_InitIndices.push_back(i*6+0);
		p_InitIndices.push_back(i*6+3);
		p_InitIndices.push_back(i*6+5);

		p_InitIndices.push_back(i*6+3);
		p_InitIndices.push_back(i*6+4);
		p_InitIndices.push_back(i*6+5);

		p_InitIndices.push_back(i*6+5);
		p_InitIndices.push_back(i*6+4);
		p_InitIndices.push_back(i*6+2);

		p_InitIndices.push_back(i*6+3);
		p_InitIndices.push_back(i*6+1);
		p_InitIndices.push_back(i*6+4);
	}
}