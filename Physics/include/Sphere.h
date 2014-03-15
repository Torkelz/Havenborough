#pragma once
#include "BoundingVolume.h"

class Sphere : public BoundingVolume
{
private:
	float					m_Radius;	// m
	float					m_SqrRadius;	// m^2

public:
	/**
	* Default constructor, sets radius to zero and position to origo.
	*/
	Sphere() :
		BoundingVolume(this)
	{
		m_BodyHandle = 0;
		setRadius(0.0f);
		m_Position = DirectX::XMFLOAT4(0.f,0.f,0.f,1.f);

		m_Type = Type::SPHERE;
		m_CollisionResponse = true;
		m_IDInBody = 0;
	}
	/**
	* Constructor, set radius and position to the inputs, calculate sqrRadius.
	* @p_radius, sphere's radius in m.
	* @p_pos, sphere's position in m.
	*/
	Sphere(float p_Radius, DirectX::XMFLOAT4 p_Pos) :
		BoundingVolume(this)
	{
		m_BodyHandle = 0;
		setRadius(p_Radius);
		m_Position = p_Pos;
		m_CollisionResponse = true;
		m_Type = Type::SPHERE;
		m_IDInBody = 0;
	}
	~Sphere()
	{

	}
	
	/**
	* Updates position for sphere with matrix.
	* @param p_translation, move the sphere in relative coordinates.
	*/
	void updatePosition(DirectX::XMFLOAT4X4 const &p_Translation) override
	{
		DirectX::XMMATRIX tempTrans;

		tempTrans = XMLoadFloat4x4(&p_Translation);

		DirectX::XMVECTOR v = XMLoadFloat4(&m_Position);

		XMStoreFloat4(&m_Position, XMVector4Transform(v, tempTrans));
	}
	/**
	* Updates position for sphere with position, used by aabb's sphere.
	* @param p_position, set current position to this.
	*/
	void setPosition(DirectX::XMVECTOR const &p_Position) override
	{
		XMStoreFloat4(&m_Position, p_Position);
	}
	/**
	* Get the radius of the sphere.
	* @return the radius of the sphere in m
	*/
	float getRadius() const
	{
		return m_Radius;
	}
	/**
	* Get the squared radius of the sphere.
	* @return the radius squared in m^2
	*/
	float getSqrRadius() const
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
	/**
	* Scales the radius of the sphere.
	* @param p_scale the X value of the vector is multiplied with the radius in m.
	*/
	void scale(const DirectX::XMVECTOR &p_Scale) override
	{
		setRadius(m_Radius * DirectX::XMVectorGetX(p_Scale));
	}

	/**
	* Does nothing since an sphere can not be rotated.
	* @param p_Rotation vector to scale the box with..
	*/
	void setRotation(DirectX::XMMATRIX const &p_Rotation) override
	{
		
	}
};