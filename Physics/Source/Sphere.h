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
	Sphere();
	Sphere(float p_radius, DirectX::XMFLOAT4 p_pos);
	~Sphere();
	
	/**
	* Updates position for sphere with matrices.
	* @p_scale, scale the sphere. 
	* @p_rotation, rotate the sphere.
	* @p_translation, move the sphere.
	*/
	void					updatePosition(XMFLOAT4X4& p_scale, XMFLOAT4X4& p_rotation, XMFLOAT4X4& p_translation );
	/**
	* Updates position for sphere with position, used by aabb's sphere.
	* @p_position, set current position to this.
	*/
	void					updatePosition(DirectX::XMFLOAT4& p_position);
	/**
	* Get the radius of the sphere.
	* @return m_radius.
	*/
	float					getRadius();
	/**
	* Get the squared radius of the sphere.
	* @return m_sqrRadius
	*/
	float					getSqrRadius();
	/**
	* Updates m_radius and m_sqrRadius.
	* @p_radius, change to this radius.
	*/
	void					setRadius(float p_radius);

	//DEBUGGING
	//void					setModel(Model p_model);
	//void					initDraw(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext);
	//void					draw( DirectX::XMFLOAT4X4& p_world, DirectX::XMFLOAT4X4& p_view, DirectX::XMFLOAT4X4& p_proj );
};

#endif SPHERE_H