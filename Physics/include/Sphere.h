#pragma once

#include "BoundingVolume.h"
//#include "Buffer.h"
//#include "Shader.h"
//#include <DirectXMath.h> included in BoundingVolume.h

//DEBUGGING
//struct CBS
//{
//	DirectX::XMMATRIX WVP;
//	DirectX::XMVECTOR color;
//};

class Sphere : public BoundingVolume
{
private:
	float					m_Radius;	// m
	float					m_SqrRadius;	// m^2

	//DEBUGGING
	//Model					m_model;
	//Buffer*				m_pBuffer;
	//Buffer*				m_pCB;
	//Shader*				m_pShader;
	//ID3D11Device*			m_pDevice;
	//ID3D11DeviceContext*	m_pDeviceContext;
	//CBS					m_cb;
	//DirectX::XMFLOAT4X4	m_translate;

public:
	/**
	* Default constructor, sets radius to zero and position to origo.
	*/
	Sphere()
	{
		setRadius(0.0f);
		m_Position = DirectX::XMFLOAT4(0.f,0.f,0.f,1.f);

		m_Type = Type::SPHERE;
	}
	/**
	* Constructor, set radius and position to the inputs, calculate sqrRadius.
	* @p_radius, sphere's radius in m.
	* @p_pos, sphere's position in m.
	*/
	Sphere(float p_Radius, DirectX::XMFLOAT4 p_Pos) : BoundingVolume()
	{
		setRadius(p_Radius);
		m_Position = p_Pos;
		
		m_Type = Type::SPHERE;
	}
	~Sphere(){
		//m_pBuffer->~Buffer();
		//m_pCB->~Buffer();
		//m_pShader->~Shader();

		//m_pBuffer			= nullptr;
		//m_pCB				= nullptr;
		//m_pShader			= nullptr;
		//m_pDevice			= nullptr;
		//m_pDeviceContext	= nullptr;
	}
	
	/**
	* Updates position for sphere with matrix.
	* @param p_translation, move the sphere in relative coordinates.
	*/
	void updatePosition( DirectX::XMFLOAT4X4& p_Translation )
	{
		m_PrevPosition = m_Position;

		DirectX::XMMATRIX tempTrans;

		tempTrans = XMLoadFloat4x4(&p_Translation);

		DirectX::XMVECTOR v = XMLoadFloat4(&m_Position); //XMVectorSet(0.f, 0.f, 0.f, 1.f);//

		XMStoreFloat4(&m_Position, XMVector4Transform(v, tempTrans));
	}
	/**
	* Updates position for sphere with position, used by aabb's sphere.
	* @param p_position, set current position to this.
	*/
	void updatePosition(DirectX::XMFLOAT4& p_Position)
	{
		m_PrevPosition = m_Position;
		m_Position = p_Position;
	}
	/**
	* Get the radius of the sphere.
	* @return the radius of the sphere in m
	*/
	float getRadius()
	{
		return m_Radius;
	}
	/**
	* Get the squared radius of the sphere.
	* @return the radius squared in m^2
	*/
	float getSqrRadius()
	{
		return m_SqrRadius;
	}
	/**
	* Updates the radius of the sphere.
	* @param p_radius the new radius in m.
	*/
	void setRadius(float p_Radius)
	{
		m_Radius = p_Radius;
		m_SqrRadius = m_Radius * m_Radius;
	}

	//DEBUGGING
	//void					setModel(Model p_model);
	//void					initDraw(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext);
	//void					draw( DirectX::XMFLOAT4X4& p_world, DirectX::XMFLOAT4X4& p_view, DirectX::XMFLOAT4X4& p_proj );
};