#pragma once

#include "BoundingVolume.h"

class OBB : public BoundingVolume
{
private:
	DirectX::XMFLOAT3 m_Extent;
	DirectX::XMFLOAT4X4 m_Rotation;
public:
	OBB()
	{
		m_Position		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_PrevPosition	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Extent		= DirectX::XMFLOAT3(0.f, 0.f, 0.f);
		m_Type			= Type::OBB;
		DirectX::XMStoreFloat4x4(&m_Rotation, DirectX::XMMatrixIdentity());
	}

	OBB(DirectX::XMFLOAT4 p_CenterPos, DirectX::XMFLOAT3 p_Extent, DirectX::XMFLOAT4X4 p_Rot)
	{
		m_Position		= p_CenterPos;
		m_PrevPosition	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Extent		= p_Extent;
		m_Type			= Type::OBB;
		m_Rotation		= p_Rot;
	}
	~OBB();


	DirectX::XMFLOAT3 getExtent()
	{
		return m_Extent;
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
	void setPosition()
	{
			
	}
};