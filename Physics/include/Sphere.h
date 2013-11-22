#ifndef SPHERE_H
#define SPHERE_H

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
	float					m_radius;
	float					m_sqrRadius;

	//DEBUGGING
	//Model					m_model;
	//Buffer*					m_pBuffer;
	//Buffer*					m_pCB;
	//Shader*					m_pShader;
	//ID3D11Device*			m_pDevice;
	//ID3D11DeviceContext*	m_pDeviceContext;
	//CBS						m_cb;
	//DirectX::XMFLOAT4X4		m_translate;

public:
	Sphere(){
		setRadius(0.0f);
		m_position = DirectX::XMFLOAT4(0.f,0.f,0.f,1.f);

		m_type = SPHERE;
	};
	Sphere(float p_radius, DirectX::XMFLOAT4 p_pos) : BoundingVolume(){
		setRadius(p_radius);
		m_position = p_pos;

		m_type = SPHERE;
	};
	~Sphere(){
		//m_pBuffer->~Buffer();
		//m_pCB->~Buffer();
		//m_pShader->~Shader();

		//m_pBuffer			= nullptr;
		//m_pCB				= nullptr;
		//m_pShader			= nullptr;
		//m_pDevice			= nullptr;
		//m_pDeviceContext	= nullptr;
	};
	
	/**
	* Updates position for sphere with matrices.
	* @p_scale, scale the sphere. 
	* @p_rotation, rotate the sphere.
	* @p_translation, move the sphere.
	*/
	void	updatePosition(DirectX::XMFLOAT4X4& p_scale, DirectX::XMFLOAT4X4& p_rotation, DirectX::XMFLOAT4X4& p_translation ){
		m_prevPosition = m_position;

		DirectX::XMMATRIX tempScale, tempRot, tempTrans;

		tempScale = XMLoadFloat4x4(&p_scale);
		tempRot = XMLoadFloat4x4(&p_rotation);
		tempTrans = XMLoadFloat4x4(&p_translation);

		DirectX::XMMATRIX tempTransform = tempScale * tempRot * tempTrans;
		DirectX::XMVECTOR v = XMLoadFloat4(&m_position);
		DirectX::XMVector4Transform(v, tempTransform);

		DirectX::XMStoreFloat4(&m_position, v);
	};
	/**
	* Updates position for sphere with position, used by aabb's sphere.
	* @p_position, set current position to this.
	*/
	void	updatePosition(DirectX::XMFLOAT4& p_position){
		m_prevPosition = m_position;
		m_position = p_position;
	};
	/**
	* Get the radius of the sphere.
	* @return m_radius.
	*/
	float	getRadius(){
		return m_radius;
	};
	/**
	* Get the squared radius of the sphere.
	* @return m_sqrRadius
	*/
	float	getSqrRadius(){
		return m_sqrRadius;
	};
	/**
	* Updates m_radius and m_sqrRadius.
	* @p_radius, change to this radius.
	*/
	void	setRadius(float p_radius){
		m_radius = p_radius;
		m_sqrRadius = m_radius * m_radius;
	};

	//DEBUGGING
	//void					setModel(Model p_model);
	//void					initDraw(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext);
	//void					draw( DirectX::XMFLOAT4X4& p_world, DirectX::XMFLOAT4X4& p_view, DirectX::XMFLOAT4X4& p_proj );
};

#endif SPHERE_H