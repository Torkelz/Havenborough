#ifndef AABB_H
#define AABB_H

#include "BoundingVolume.h"
#include "Sphere.h"
/*#include "Buffer.h"
#include "Shader.h"*/
#include <vector>
//#include <DirectXMath.h>// included in BoundingVolume.h

//DEBUGGING
//struct CB
//{
//	XMMATRIX WVP;
//	XMVECTOR color;
//};

class AABB : public BoundingVolume
{
private:
	DirectX::XMFLOAT4	m_bottom;
	DirectX::XMFLOAT4	m_top;
	DirectX::XMFLOAT4	m_bounds[8];
	std::vector<int>	m_indices;
	Sphere				m_sphere;
	DirectX::XMFLOAT4	m_halfDiagonal;

	////DEBUGGING
	/*Buffer*				m_pBuffer;
	Buffer*				m_pCB;
	Buffer*				m_pIndexBuffer;
	Shader*				m_pShader;
	ID3D11Device*		m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	CB					m_cb;
	DirectX::XMFLOAT4X4	m_translate;
	DirectX::XMFLOAT4	m_color;*/
	
public:
	AABB(){};
	/**
	* ## MUST BE BUILT IN MODEL SPACE! ##
	* @p_top is the most positive corner in all axis.
	* @p_bot is the least positive corner in all axis.
	*/
	AABB( DirectX::XMFLOAT4 p_top, DirectX::XMFLOAT4 p_bot) : BoundingVolume() {
		m_top		= p_top;// + vec3(1.0f, 1.0f, 0.0f);
		m_bottom	= p_bot;// + vec3(-1.0f, -1.0f, 0.0f);
		
		m_bounds[0] = m_bottom;
		m_bounds[7] = m_top;

		m_position = DirectX::XMFLOAT4(	m_bottom.x + ((m_top.x - m_bottom.x) / 2) , 
								m_bottom.y + ((m_top.y - m_bottom.y) / 2) , 
								m_bottom.z + ((m_top.z - m_bottom.z) / 2) ,
								0.0f );
		m_type		= AABBOX;

		initialize();
	};
	~AABB(){
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
	};
	
	/**
	* Initialize the AABB
	*/
	void initialize()
	{
		calculateBounds();
	};
	/**
	* Calculate corners, half diagonal and create bounding sphere for AABB.
	*/
	void calculateBounds()
	{
		using namespace DirectX;
		m_position = XMFLOAT4(	m_bounds[0].x + ((m_bounds[7].x - m_bounds[0].x) / 2) , 
								m_bounds[0].y + ((m_bounds[7].y - m_bounds[0].y) / 2) , 
								m_bounds[0].z + ((m_bounds[7].z - m_bounds[0].z) / 2) ,
								0.0f );

		m_bounds[1] = XMFLOAT4( m_bounds[7].x,		m_bounds[0].y,		m_bounds[0].z, 1.0f ); // Xyz
		m_bounds[2] = XMFLOAT4( m_bounds[0].x,		m_bounds[7].y,		m_bounds[0].z, 1.0f ); // xYz
		m_bounds[3] = XMFLOAT4( m_bounds[7].x,		m_bounds[7].y,		m_bounds[0].z, 1.0f ); // XYz
		m_bounds[4] = XMFLOAT4( m_bounds[0].x,		m_bounds[0].y,		m_bounds[7].z, 1.0f ); // xyZ
		m_bounds[5] = XMFLOAT4( m_bounds[7].x,		m_bounds[0].y,		m_bounds[7].z, 1.0f ); // XyZ
		m_bounds[6] = XMFLOAT4( m_bounds[0].x,		m_bounds[7].y,		m_bounds[7].z, 1.0f ); // xYZ

		XMVECTOR vBot, vTop, vDiag;

		vBot = XMLoadFloat4(&m_bounds[0]);
		vTop = XMLoadFloat4(&m_bounds[7]);
		
		
		vDiag = vTop - vBot;
		vDiag *= 0.5f;

		m_sphere.setRadius(XMVector3Length(vDiag).m128_f32[0]);
		m_sphere.updatePosition(m_position);

		DirectX::XMStoreFloat4(&m_halfDiagonal, vDiag);
	};
	/**
	* Updates position for AABB with matrices.
	* @p_scale, scale the AABB. 
	* @p_rotation, rotate the AABB.
	* @p_translation, move the AABB.
	*/
	void				updatePosition( DirectX::XMFLOAT4X4& p_scale, DirectX::XMFLOAT4X4& p_rotation, DirectX::XMFLOAT4X4& p_translate ){
		DirectX::XMMATRIX tempScale, tempRot, tempTrans;
	
		tempScale = XMLoadFloat4x4(&p_scale);
		tempRot = XMLoadFloat4x4(&p_rotation);
		tempTrans = XMLoadFloat4x4(&p_translate);
	
		DirectX::XMMATRIX tempTransform = tempScale * tempRot * tempTrans;
		DirectX::XMVECTOR vBot, vTop;
		vBot = DirectX::XMLoadFloat4(&m_bottom);
		vTop = DirectX::XMLoadFloat4(&m_top);
	
		vBot = DirectX::XMVector4Transform(vBot, tempTransform);
		vTop = DirectX::XMVector4Transform(vTop, tempTransform);
		DirectX::XMStoreFloat4(&m_bounds[0], vBot);
		DirectX::XMStoreFloat4(&m_bounds[7], vTop);
	
		calculateBounds();
	};
	/**
	* @return m_top, return top corner
	*/
	DirectX::XMFLOAT4*	getMax(){
		return &m_bounds[7];
	};
	/**
	* @return m_bottom, return bottom corner
	*/
	DirectX::XMFLOAT4*	getMin(){
		return &m_bounds[0];
	};
	/**
	* @return m_halfDiagonal, return vector from center to top corner.
	*/
	DirectX::XMFLOAT4*	getHalfDiagonal(){
		return &m_halfDiagonal;
	};
	/**
	* @return m_sphere, AABB's sphere
	*/
	Sphere*	getSphere(){
		return &m_sphere;
	};

	//DEBUGGING
	/*void				buildCubeIndices( int offset );
	void				initDraw(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext, DirectX::XMFLOAT4 p_color);
	void				draw( DirectX::XMFLOAT4X4& p_world, DirectX::XMFLOAT4X4& p_view, DirectX::XMFLOAT4X4& p_proj );*/
};

#endif