#pragma once
#include "Sphere.h"

#include <vector>

class AABB : public BoundingVolume
{
private:
	DirectX::XMFLOAT4	m_Bottom;
	DirectX::XMFLOAT4	m_Top;
	DirectX::XMFLOAT4	m_Bounds[8];
	Sphere				m_Sphere;
	DirectX::XMFLOAT4	m_HalfDiagonal;
	DirectX::XMFLOAT4	m_Size;
	
public:
	AABB()
	{
		m_BodyHandle = 0;
		m_Position = DirectX::XMFLOAT4(0.f,0.f,0.f,1.f);
		m_Size = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Type = Type::AABBOX;
		m_CollisionResponse = true;
		calculateBounds();
		m_IDInBody = 0;
	}
	/**
	 * @param p_CenterPos the position in world space in m
	 * @param p_Size the halfsize of the box in m
	 */
	AABB(DirectX::XMFLOAT4 p_CenterPos, DirectX::XMFLOAT4 p_Size) : BoundingVolume()
	{
		m_BodyHandle = 0;
		m_Position = p_CenterPos;
		m_Size.x = p_Size.x;
		m_Size.y = p_Size.y;
		m_Size.z = p_Size.z;
		m_Size.w = p_Size.w;
		
		m_Type		= Type::AABBOX;

		m_Sphere.setPosition(XMLoadFloat4(&p_CenterPos));

		calculateBounds();
		m_CollisionResponse = true;
		m_IDInBody = 0;
	}
	/**
	* Destructor
	*/
	~AABB()
	{
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

		XMVECTOR vBot, vTop, vDiag;

		vBot = XMLoadFloat4(&m_Bounds[0]);
		vTop = XMLoadFloat4(&m_Bounds[7]);
		
		vDiag = vTop - vBot;
		vDiag *= 0.5f;

		m_Sphere.setRadius(XMVector3Length(vDiag).m128_f32[0]);

		DirectX::XMStoreFloat4(&m_HalfDiagonal, vDiag);
	}
	/**
	* Updates position for AABB with translation matrix.
	* @param p_translation, move the AABB in relative coordinates.
	*/
	void updatePosition(DirectX::XMFLOAT4X4 const &p_Translation)
	{
		DirectX::XMMATRIX tempTrans;

		tempTrans = XMLoadFloat4x4(&p_Translation);

		DirectX::XMVECTOR tPos;
		tPos = XMLoadFloat4(&m_Position);

		tPos = XMVector4Transform(tPos, tempTrans);

		XMStoreFloat4(&m_Position, tPos);

		calculateBounds();
		m_Sphere.setPosition(tPos);
	}

	void setPosition(DirectX::XMVECTOR const &p_newPosition) override
	{
		DirectX::XMStoreFloat4(&m_Position, p_newPosition);
		m_Sphere.setPosition(p_newPosition);
	}

	/**
	 * Sets a new size for AABB and recalculates.
	 * @param p_Size, new size.
	 */
	void setSize(DirectX::XMFLOAT4 p_Size)
	{
		m_Size = p_Size;
		calculateBounds();
	}
	/**
	* @return the top corner in world coordinates m
	*/
	DirectX::XMFLOAT4 getMax() const 
	{
		return getBoundWorldCoordAt(7);
	}
	/**
	* @return the bottom corner in world coordinates m
	*/
	DirectX::XMFLOAT4 getMin() const
	{
		return getBoundWorldCoordAt(0);
	}
	/**
	* @return a vector from center to top corner m
	*/
	DirectX::XMFLOAT4 getHalfDiagonal() const
	{
		return m_HalfDiagonal;
	}
	/**
	* @return the sphere that surround the AABB
	*/
	Sphere	getSphere() const 
	{
		return m_Sphere;
	}
	/**
	 * Return a corner at the index specified.
	 * 
	 * @param p_Index index number in m_Bounds list
	 * @return a XMFLOAT4 corner.
	 */
	DirectX::XMFLOAT4 getBoundAt(unsigned p_Index) const
	{
		return m_Bounds[p_Index];
	}
	/**
	 * Return a corner in world coordinates at the index specified.
	 * 
	 * @param p_Index index number in m_Bounds list
	 * @return a XMFLOAT4 corner.
	 */
	DirectX::XMFLOAT4 getBoundWorldCoordAt(unsigned p_Index) const
	{
		return DirectX::XMFLOAT4(m_Bounds[p_Index].x + m_Position.x,m_Bounds[p_Index].y + m_Position.y, m_Bounds[p_Index].z + m_Position.z, 1.f);
	}
	/**
	* Scales the ABBB.
	* @param p_scale vector to scale the box with..
	*/
	void scale(const DirectX::XMVECTOR &p_Scale) override
	{
		DirectX::XMMATRIX m = DirectX::XMMatrixScalingFromVector(p_Scale);
		DirectX::XMVECTOR s = XMLoadFloat4(&m_Size);
		s = DirectX::XMVector4Transform(s, m);
		DirectX::XMStoreFloat4(&m_Size, s);
		calculateBounds();
	}
	/**
	* Does nothing since an AABB can not be rotated.
	* @param p_Rotation vector to scale the box with..
	*/
	void setRotation(DirectX::XMMATRIX const &p_Rotation) override
	{
		
	}
};
