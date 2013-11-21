#include "AABB.h"

using namespace DirectX;

AABB::AABB(XMFLOAT4 p_top, XMFLOAT4 p_bot) : BoundingVolume()
{
	m_top		= p_top;// + vec3(1.0f, 1.0f, 0.0f);
	m_bottom	= p_bot;// + vec3(-1.0f, -1.0f, 0.0f);
	
	m_bounds[0] = m_bottom;
	m_bounds[7] = m_top;

	m_position = XMFLOAT4(	m_bottom.x + ((m_top.x - m_bottom.x) / 2) , 
							m_bottom.y + ((m_top.y - m_bottom.y) / 2) , 
							m_bottom.z + ((m_top.z - m_bottom.z) / 2) ,
							0.0f );
	m_type		= AABBOX;

	initialize();
}
AABB::~AABB()
{
	/*m_pBuffer->~Buffer();
	m_pCB->~Buffer();
	m_pIndexBuffer->~Buffer();
	m_pShader->~Shader();

	m_pBuffer			= nullptr;
	m_pCB				= nullptr;
	m_pIndexBuffer		= nullptr;
	m_pShader			= nullptr;
	m_pDevice			= nullptr;
	m_pDeviceContext	= nullptr;*/
}

void AABB::initialize()
{
	calculateBounds();
}

void AABB::calculateBounds()
{
	m_position = XMFLOAT4(	m_bounds[0].x + ((m_bounds[7].x - m_bounds[0].x) / 2) , 
							m_bounds[0].y + ((m_bounds[7].y - m_bounds[0].y) / 2) , 
							m_bounds[0].z + ((m_bounds[7].z - m_bounds[0].z) / 2) ,
							0.0f );

	m_bounds[1] = XMFLOAT4( m_bounds[7].x,		m_bounds[0].y,		m_bounds[0].z, 0.0f ); // Xyz
	m_bounds[2] = XMFLOAT4( m_bounds[0].x,		m_bounds[7].y,		m_bounds[0].z, 0.0f ); // xYz
	m_bounds[3] = XMFLOAT4( m_bounds[7].x,		m_bounds[7].y,		m_bounds[0].z, 0.0f ); // XYz
	m_bounds[4] = XMFLOAT4( m_bounds[0].x,		m_bounds[0].y,		m_bounds[7].z, 0.0f ); // xyZ
	m_bounds[5] = XMFLOAT4( m_bounds[7].x,		m_bounds[0].y,		m_bounds[7].z, 0.0f ); // XyZ
	m_bounds[6] = XMFLOAT4( m_bounds[0].x,		m_bounds[7].y,		m_bounds[7].z, 0.0f ); // xYZ

	XMVECTOR vBot, vTop, vDiag;

	vBot = XMLoadFloat4(&m_bounds[0]);
	vTop = XMLoadFloat4(&m_bounds[7]);

	vDiag = vTop - vBot;
	vDiag *= 0.5f;

	m_sphere.setRadius(XMVector3Length(vDiag).m128_f32[0]);
	m_sphere.updatePosition(m_position);

	XMStoreFloat4(&m_halfDiagonal, vDiag);
}

void AABB::updatePosition(XMFLOAT4X4& p_scale, XMFLOAT4X4& p_rotation, XMFLOAT4X4& p_translate)
{
	XMMATRIX tempScale, tempRot, tempTrans;

	tempScale = XMLoadFloat4x4(&p_scale);
	tempRot = XMLoadFloat4x4(&p_rotation);
	tempTrans = XMLoadFloat4x4(&p_translate);

	XMMATRIX tempTransform = tempScale * tempRot * tempTrans;
	XMVECTOR vBot, vTop;
	vBot = XMLoadFloat4(&m_bottom);
	vTop = XMLoadFloat4(&m_top);

	vBot = XMVector4Transform(vBot, tempTransform);
	vTop = XMVector4Transform(vTop, tempTransform);
	XMStoreFloat4(&m_bounds[0], vBot);
	XMStoreFloat4(&m_bounds[7], vTop);

	calculateBounds();
}

XMFLOAT4* AABB::getMax()
{
	return &m_bounds[7];
}

XMFLOAT4* AABB::getMin()
{
	return &m_bounds[0];
}

XMFLOAT4* AABB::getHalfDiagonal()
{
	return &m_halfDiagonal;
}

Sphere* AABB::getSphere()
{
	return &m_sphere;
}

////## DIRECT3D DEBUG DRAW ##
//void AABB::buildCubeIndices(int offset)
//{
//	// Cube Composition
//	/*		  +----+  6    7	| Refered to as
//			 /    /|			| indices of the
//			+----+ |  4    5	| vertices below
//	2	 3	|    | +			
//			|    |/				
//	0	 1	+----+								*/
//
//	// Right/Left
//	for( int i = 0; i < 8; i++ )
//	{
//		m_indices.push_back( i + offset );
//	}
//	// Froward/Backward
//	for( int i = 0; i < 4; i++ )
//	{
//		m_indices.push_back( i + offset );
//		m_indices.push_back( i + offset + 4 );
//	}
//	// Up/Down
//	for( int i = 0; i < 2; i++ )
//	{
//		m_indices.push_back( i + offset );
//		m_indices.push_back( i + offset + 2 );
//		m_indices.push_back( i + offset + 4 );
//		m_indices.push_back( i + offset + 2 + 4 );
//	}
//}
//
//void AABB::initDraw(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext, XMFLOAT4 p_color)
//{
//	m_pDevice = p_pDevice;
//	m_pDeviceContext = p_pDeviceContext;
//	m_color = p_color;
//
//	//m_sphere.initDraw(m_pDevice, m_pDeviceContext);
//
//	buildCubeIndices(0);
//
//	BufferInitDesc desc;
//	desc.type				= VERTEX_BUFFER;
//	desc.numElements		= 8;
//	desc.elementSize		= sizeof( XMFLOAT4 );
//	desc.usage				= BUFFER_DEFAULT;
//	desc.initData			= m_bounds;
//
//	m_pBuffer = new Buffer();
//	m_pBuffer->init(p_pDevice, p_pDeviceContext, desc);
//
//	BufferInitDesc cbDesc;	
//
//	cbDesc.elementSize = sizeof(CB);
//	cbDesc.initData = NULL;
//	cbDesc.numElements = 1;
//	cbDesc.type = CONSTANT_BUFFER_VS;
//	cbDesc.usage = BUFFER_DEFAULT;
//	
//	m_pCB = new Buffer();
//	m_pCB->init(p_pDevice, p_pDeviceContext, cbDesc);
//
//	int						temp[24];
//	for (UINT i = 0; i < 24; i++)
//	{
//		temp[i]				= m_indices.at(i);
//	}
//
//	desc.type				= INDEX_BUFFER;
//	desc.numElements		= 24;
//	desc.elementSize		= sizeof(UINT);
//	desc.usage				= BUFFER_DEFAULT;
//	desc.initData			= &temp;
//
//	m_pIndexBuffer = new Buffer();
//	m_pIndexBuffer->init(p_pDevice, p_pDeviceContext, desc);
//
//	D3D11_INPUT_ELEMENT_DESC inputDesc[] = 
//	{
//		{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
//	};
//
//	m_pShader = new Shader();
//	m_pShader->init(p_pDevice, p_pDeviceContext, 1);
//	m_pShader->compileAndCreateShaderFromFile(L"VertexShader.fx", "main","vs_5_0", VERTEX_SHADER , inputDesc);
//	m_pShader->compileAndCreateShaderFromFile(L"PixelShader.fx", "main", "ps_5_0", PIXEL_SHADER, NULL);
//}
//
//void AABB::draw(XMFLOAT4X4& p_world, XMFLOAT4X4& p_view, XMFLOAT4X4& p_proj)
//{
//	XMMATRIX matTranslate = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
//	XMMATRIX matWorld, matView, matProj;
//	matWorld	= XMLoadFloat4x4(&p_world);
//	matView		= XMLoadFloat4x4(&p_view);
//	matProj		= XMLoadFloat4x4(&p_proj);
//	
//	XMMATRIX WorldViewProj;
//	WorldViewProj	= XMMatrixMultiply(matWorld, matTranslate);
//	WorldViewProj	= XMMatrixMultiply(WorldViewProj, matView );
//	WorldViewProj	= XMMatrixMultiply(WorldViewProj, matProj );
//	
//	XMStoreFloat4x4(&m_translate, matTranslate);
//
//	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
//
//	m_cb.color = XMVectorSet(m_color.x, m_color.y, m_color.z, m_color.w);
//	m_cb.WVP = XMMatrixTranspose(WorldViewProj);
//	m_pCB->apply(0);
//	m_pDeviceContext->UpdateSubresource(m_pCB->getBufferPointer(), 0, NULL, &m_cb, 0, 0);
//	
//	m_pIndexBuffer->apply(0);
//	m_pBuffer->apply(0);
//	m_pShader->setShaders();
//
//	m_pDeviceContext->DrawIndexed(24, 0, 0);
//	
//	//m_sphere.draw(p_world, p_view, p_proj);
//}