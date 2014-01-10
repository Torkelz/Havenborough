#pragma once

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
	DirectX::XMFLOAT4	m_Bottom;
	DirectX::XMFLOAT4	m_Top;
	DirectX::XMFLOAT4	m_Bounds[8];
	std::vector<int>	m_Indices;
	Sphere				m_Sphere;
	DirectX::XMFLOAT4	m_HalfDiagonal;
	DirectX::XMFLOAT4	m_Size;

	////DEBUGGING
	/*Buffer*			m_pBuffer;
	Buffer*				m_pCB;
	Buffer*				m_pIndexBuffer;
	Shader*				m_pShader;
	ID3D11Device*		m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	CB					m_cb;
	DirectX::XMFLOAT4X4	m_translate;
	DirectX::XMFLOAT4	m_color;*/
	
public:
	AABB(){}
	/**
	* ## MUST BE BUILT IN MODEL SPACE! ##
	* @param p_top is the most positive corner in all axis.
	* @param p_bot is the least positive corner in all axis.
	*/
	AABB( DirectX::XMFLOAT4 p_CenterPos, DirectX::XMFLOAT4 p_Size) : BoundingVolume()
	{
		m_Position = p_CenterPos;
		m_Size.x = p_Size.x;
		m_Size.y = p_Size.y;
		m_Size.z = p_Size.z;
		m_Size.w = p_Size.w;
		
		m_Type		= Type::AABBOX;

		calculateBounds();
	}
	//AABB( DirectX::XMFLOAT4 p_Bot, DirectX::XMFLOAT4 p_Top) : BoundingVolume()
	//{
	//	m_Top		= p_Top;
	//	m_Bottom	= p_Bot;
	//
	//	m_Bounds[0] = m_Bottom;
	//	m_Bounds[7] = m_Top;

	//	m_Position = DirectX::XMFLOAT4(	m_Bottom.x + ((m_Top.x - m_Bottom.x) / 2) , 
	//									m_Bottom.y + ((m_Top.y - m_Bottom.y) / 2) , 
	//									m_Bottom.z + ((m_Top.z - m_Bottom.z) / 2) ,
	//									1.0f );
	//	m_Type		= Type::AABBOX;

	//	calculateBounds();
	//}
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
	}
	
	/**
	* Initialize the AABB
	*/
	void initialize()
	{
		calculateBounds();
	}
	/**
	* Calculate corners, half diagonal and create bounding sphere for AABB.
	*/
	void calculateBounds()
	{
		using namespace DirectX;

		m_Bounds[0] = XMFLOAT4(- m_Size.x, - m_Size.y, - m_Size.z, 1.f);
		m_Bounds[1] = XMFLOAT4(+ m_Size.x, - m_Size.y, - m_Size.z, 1.f);
		m_Bounds[2] = XMFLOAT4(- m_Size.x, + m_Size.y, - m_Size.z, 1.f);
		m_Bounds[3] = XMFLOAT4(+ m_Size.x, + m_Size.y, - m_Size.z, 1.f);
		m_Bounds[4] = XMFLOAT4(- m_Size.x, - m_Size.y, + m_Size.z, 1.f);
		m_Bounds[5] = XMFLOAT4(+ m_Size.x, - m_Size.y, + m_Size.z, 1.f);
		m_Bounds[6] = XMFLOAT4(- m_Size.x, + m_Size.y, + m_Size.z, 1.f);
		m_Bounds[7] = XMFLOAT4(+ m_Size.x, + m_Size.y, + m_Size.z, 1.f);

		//using namespace DirectX;
		//m_Position = XMFLOAT4(	m_Bounds[0].x + ((m_Bounds[7].x - m_Bounds[0].x) / 2) , 
		//						m_Bounds[0].y + ((m_Bounds[7].y - m_Bounds[0].y) / 2) , 
		//						m_Bounds[0].z + ((m_Bounds[7].z - m_Bounds[0].z) / 2) ,
		//						1.0f );

		//m_Bounds[1] = XMFLOAT4( m_Bounds[7].x,		m_Bounds[0].y,		m_Bounds[0].z, 1.0f ); // Xyz
		//m_Bounds[2] = XMFLOAT4( m_Bounds[0].x,		m_Bounds[7].y,		m_Bounds[0].z, 1.0f ); // xYz
		//m_Bounds[3] = XMFLOAT4( m_Bounds[7].x,		m_Bounds[7].y,		m_Bounds[0].z, 1.0f ); // XYz
		//m_Bounds[4] = XMFLOAT4( m_Bounds[0].x,		m_Bounds[0].y,		m_Bounds[7].z, 1.0f ); // xyZ
		//m_Bounds[5] = XMFLOAT4( m_Bounds[7].x,		m_Bounds[0].y,		m_Bounds[7].z, 1.0f ); // XyZ
		//m_Bounds[6] = XMFLOAT4( m_Bounds[0].x,		m_Bounds[7].y,		m_Bounds[7].z, 1.0f ); // xYZ

		XMVECTOR vBot, vTop, vDiag;

		vBot = XMLoadFloat4(&m_Bounds[0]);
		vTop = XMLoadFloat4(&m_Bounds[7]);
		
		vDiag = vTop - vBot;
		vDiag *= 0.5f;

		m_Sphere.setRadius(XMVector3Length(vDiag).m128_f32[0]);
		m_Sphere.updatePosition(m_Position);

		DirectX::XMStoreFloat4(&m_HalfDiagonal, vDiag);
	}
	/**
	* Updates position for AABB with translation matrix.
	* @param p_translation, move the AABB in relative coordinates.
	*/
	void updatePosition(DirectX::XMFLOAT4X4& p_Translation)
	{
		DirectX::XMMATRIX tempTrans;

		tempTrans = XMLoadFloat4x4(&p_Translation);

		DirectX::XMVECTOR tPos;
		tPos = XMLoadFloat4(&m_Position);

		tPos = XMVector4Transform(tPos, tempTrans);

		XMStoreFloat4(&m_Position, tPos);

		calculateBounds();
	}
	/**
	* @return the top corner
	*/
	DirectX::XMFLOAT4* getMax()
	{
		return &m_Bounds[7];
	}
	/**
	* @return the bottom corner
	*/
	DirectX::XMFLOAT4* getMin()
	{
		return &m_Bounds[0];
	}
	/**
	* @return a vector from center to top corner.
	*/
	DirectX::XMFLOAT4* getHalfDiagonal()
	{
		return &m_HalfDiagonal;
	}
	/**
	* @return the sphere that surround the AABB
	*/
	Sphere*	getSphere()
	{
		return &m_Sphere;
	}

	//DEBUGGING
	/*void				buildCubeIndices( int offset );
	void				initDraw(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext, DirectX::XMFLOAT4 p_color);
	void				draw( DirectX::XMFLOAT4X4& p_world, DirectX::XMFLOAT4X4& p_view, DirectX::XMFLOAT4X4& p_proj );*/
};
