#pragma once

#include "BoundingVolume.h"
#include "Sphere.h"

class OBB : public BoundingVolume
{
private:
	DirectX::XMFLOAT3X3	m_Axes; // Local x-,y and z-axes
	DirectX::XMFLOAT4	m_Extents; //Positive half-width extents oo OBB along each axis
	DirectX::XMFLOAT4X4 m_Rotation;
	Sphere				m_Sphere;
	
public:
	OBB()
	{
		m_Position		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f); //OBB Center Point
		m_PrevPosition	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Extents		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Type			= Type::OBB;
		DirectX::XMStoreFloat3x3(&m_Axes, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_Rotation, DirectX::XMMatrixIdentity());
		m_Sphere		= Sphere();
	}

	OBB(DirectX::XMFLOAT4 p_CenterPos, DirectX::XMFLOAT4 p_Corner)
	{
		m_Position		= p_CenterPos; //OBB Center Point.
		m_PrevPosition	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Type			= Type::OBB;
		DirectX::XMStoreFloat3x3(&m_Axes, DirectX::XMMatrixIdentity());
		calcExtent(p_Corner);
		m_Extents = p_Corner;

		//Sphere
		DirectX::XMVECTOR extentVector = DirectX::XMLoadFloat4(&m_Extents);
		DirectX::XMVECTOR length = DirectX::XMVector3Length(extentVector);
		float radius;
		DirectX::XMStoreFloat(&radius, length);
		m_Sphere		= Sphere(radius, p_CenterPos);

	}
	~OBB()
	{
	}

	void updatePosition(DirectX::XMFLOAT4X4& p_Translation) override
	{
		DirectX::XMMATRIX mTrans;
		mTrans = DirectX::XMLoadFloat4x4(&p_Translation);

		DirectX::XMVECTOR vCenterPos;
		vCenterPos = DirectX::XMLoadFloat4(&m_Position);
		vCenterPos = DirectX::XMVector4Transform(vCenterPos, mTrans);

		DirectX::XMStoreFloat4(&m_Position, vCenterPos);

		m_Sphere.updatePosition(m_Position);
		
	}

	DirectX::XMFLOAT3X3 getAxes()
	{
		return m_Axes;
	}

	DirectX::XMFLOAT4 getExtents()
	{
		return m_Extents;
	}

	DirectX::XMFLOAT4X4 getRotationMatrix()
	{
		return m_Rotation;
	}

	DirectX::XMFLOAT3 getSize()
	{
		using DirectX::operator*;
		DirectX::XMVECTOR temp = DirectX::XMLoadFloat4(&m_Extents);
		temp = temp * 2.0f;
		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, temp);
		return result;
	}

	DirectX::XMFLOAT4X4 getInvRotation()
	{
		DirectX::XMFLOAT4X4 inverse;
		DirectX::XMMATRIX tempRot = DirectX::XMMATRIX(DirectX::XMLoadFloat4x4(&m_Rotation));
		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(tempRot);

		tempRot = DirectX::XMMatrixInverse(&determinant, tempRot);
		DirectX::XMStoreFloat4x4(&inverse, tempRot);

		return inverse;
	}

	Sphere getSphere()
	{
		return m_Sphere;
	}

	void setExtent(const DirectX::XMFLOAT4 &p_Extents)
	{
		m_Extents = p_Extents;
	}

	void setRotationMatrix(const DirectX::XMFLOAT4X4 &p_Rot)
	{
		m_Rotation = p_Rot;
		updateRotation();
	}

private:
	void updateRotation()
	{
		DirectX::XMVECTOR tempExtent = DirectX::XMLoadFloat4(&m_Extents);
		DirectX::XMMATRIX tempRot = DirectX::XMLoadFloat4x4(&m_Rotation);
		DirectX::XMMATRIX tAxes= XMLoadFloat3x3(&m_Axes);
		tAxes = tempRot * tAxes;
		tempExtent = DirectX::XMVector4Transform(tempExtent, tempRot);

		DirectX::XMStoreFloat3x3(&m_Axes, tAxes);
		DirectX::XMStoreFloat4(&m_Extents, tempExtent);
	}

	void calcExtent(const DirectX::XMFLOAT4 &p_Corner)
	{
		DirectX::XMVECTOR tCorner = DirectX::XMLoadFloat4(&p_Corner);
		DirectX::XMVECTOR tPosition = DirectX::XMLoadFloat4(&m_Position);
		DirectX::XMVECTOR tExtent;
		
		using DirectX::operator-;
		tExtent = tCorner - tPosition;

		DirectX::XMStoreFloat4(&m_Extents, tExtent);
		m_Extents.x = fabs(m_Extents.x);
		m_Extents.y = fabs(m_Extents.y);
		m_Extents.z = fabs(m_Extents.z);
		m_Extents.w = fabs(m_Extents.w);

	}
};