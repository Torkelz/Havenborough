#pragma once

#include "BoundingVolume.h"
#include "Sphere.h"

class OBB : public BoundingVolume
{
private:
	DirectX::XMFLOAT3	m_Extent;
	DirectX::XMFLOAT4X4 m_Rotation;
	Sphere				m_Sphere;

public:
	OBB()
	{
		m_Position		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_PrevPosition	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Extent		= DirectX::XMFLOAT3(0.f, 0.f, 0.f);
		m_Type			= Type::OBB;
		DirectX::XMStoreFloat4x4(&m_Rotation, DirectX::XMMatrixIdentity());
		m_Sphere		= Sphere();
	}

	OBB(DirectX::XMFLOAT4 p_CenterPos, DirectX::XMFLOAT3 p_Extent, DirectX::XMFLOAT4X4 p_Rot)
	{
		m_Position		= p_CenterPos;
		m_PrevPosition	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Extent		= p_Extent;
		m_Type			= Type::OBB;
		m_Rotation		= p_Rot;

		DirectX::XMVECTOR extentVector = DirectX::XMVECTOR(DirectX::XMLoadFloat3(&m_Extent));
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

	DirectX::XMFLOAT3 getExtent()
	{
		return m_Extent;
	}

	DirectX::XMFLOAT4X4 getRotationMatrix()
	{
		return m_Rotation;
	}

	DirectX::XMFLOAT3 getSize()
	{
		using DirectX::operator*;
		DirectX::XMVECTOR temp = DirectX::XMVECTOR(DirectX::XMLoadFloat3(&m_Extent));
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

	void setExtent(const DirectX::XMFLOAT3 &p_Extent)
	{
		m_Extent = p_Extent;
	}

	void setRotationMatrix(const DirectX::XMFLOAT4X4 &p_Rot)
	{
		m_Rotation = p_Rot;
	}
};