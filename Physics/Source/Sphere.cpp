//#include "Sphere.h"
//DEBUGGING
//void Sphere::setModel(Model p_model)
//{
//	m_model = p_model;
//}
//
//void Sphere::initDraw(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext)
//{
//	m_pDevice = p_pDevice;
//	m_pDeviceContext = p_pDeviceContext;
//
//	BufferInitDesc desc;
//	desc.type				= VERTEX_BUFFER;
//	desc.numElements		= m_model.m_triangles.size() * 4;
//	desc.elementSize		= sizeof( float )  * 4;
//	desc.usage				= BUFFER_DEFAULT;
//	desc.initData			= &m_model.m_triangles[0];
//
//	m_pBuffer = new Buffer();
//	m_pBuffer->init(p_pDevice, p_pDeviceContext, desc);
//
//	BufferInitDesc cbDesc;
//
//	cbDesc.elementSize = sizeof(CBS);
//	cbDesc.initData = NULL;
//	cbDesc.numElements = 1;
//	cbDesc.type = CONSTANT_BUFFER_VS;
//	cbDesc.usage = BUFFER_DEFAULT;
//	
//	m_pCB = new Buffer();
//	m_pCB->init(p_pDevice, p_pDeviceContext, cbDesc);
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
//void Sphere::draw(XMFLOAT4X4& p_world, XMFLOAT4X4& p_view, XMFLOAT4X4& p_proj)
//{
//	XMMATRIX matTranslate = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
//	XMMATRIX matScale = XMMatrixScaling(m_radius, m_radius, m_radius);
//	XMMATRIX matWorld, matView, matProj;
//	matWorld	= XMLoadFloat4x4(&p_world);
//	matView		= XMLoadFloat4x4(&p_view);
//	matProj		= XMLoadFloat4x4(&p_proj);
//	
//	XMMATRIX WorldViewProj;
//	WorldViewProj	= XMMatrixMultiply(matWorld, matScale);
//	WorldViewProj	= XMMatrixMultiply(WorldViewProj, matTranslate);
//	WorldViewProj	= XMMatrixMultiply(WorldViewProj, matView );
//	WorldViewProj	= XMMatrixMultiply(WorldViewProj, matProj );
//	
//	XMStoreFloat4x4(&m_translate, matTranslate);
//
//	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//	m_cb.color = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
//	m_cb.WVP = XMMatrixTranspose(WorldViewProj);
//	m_pCB->apply(0);
//	m_pDeviceContext->UpdateSubresource(m_pCB->getBufferPointer(), 0, NULL, &m_cb, 0, 0);
//	
//	m_pBuffer->apply(0);
//	m_pShader->setShaders();
//
//	m_pDeviceContext->Draw(m_model.m_triangles.size() * 4, 0);
//}