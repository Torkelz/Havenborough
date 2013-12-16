#pragma once

#include <DirectXMath.h>

class StaticVertex
{
public:
	DirectX::XMFLOAT4 m_Position;
	DirectX::XMFLOAT3 m_Normal;
	DirectX::XMFLOAT2 m_UV;
	DirectX::XMFLOAT3 m_Tangent;
	DirectX::XMFLOAT3 m_Binormal;
	StaticVertex(){}
	StaticVertex(DirectX::XMFLOAT3 _position,
		DirectX::XMFLOAT3 _normal,
		DirectX::XMFLOAT2 _uv,
		DirectX::XMFLOAT3 _tangent)
	{
		m_Position = DirectX::XMFLOAT4(_position.x,_position.y,_position.z,1.0f);
		m_Normal = _normal;
		m_UV = _uv;
		m_Tangent = _tangent;

		//might be wrong
		DirectX::XMStoreFloat3(&m_Binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&m_Tangent),DirectX::XMLoadFloat3(&m_Normal)));
	}
};

class AnimatedVertex
{
public:
	DirectX::XMFLOAT4 m_Position;
	DirectX::XMFLOAT3 m_Normal;
	DirectX::XMFLOAT2 m_UV;
	DirectX::XMFLOAT3 m_Tangent;
	DirectX::XMFLOAT3 m_Binormal;
	DirectX::XMFLOAT3 m_Weights;
	unsigned int m_BoneId[4];
	AnimatedVertex() {}
	AnimatedVertex(
		DirectX::XMFLOAT3 _position,
		DirectX::XMFLOAT3 _normal,
		DirectX::XMFLOAT2 _uv,
		DirectX::XMFLOAT3 _tangent,
		DirectX::XMFLOAT3 _weights,
		DirectX::XMFLOAT4 _boneId)
	{
		m_Position = DirectX::XMFLOAT4(_position.x,_position.y,_position.z,1.0f);
		m_Normal = _normal;
		m_UV = _uv;
		m_Tangent = _tangent;

		//might be wrong
		DirectX::XMStoreFloat3(&m_Binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&m_Tangent),DirectX::XMLoadFloat3(&m_Normal)));

		m_Weights = _weights;
		m_BoneId[0] = unsigned int(_boneId.x);
		m_BoneId[1] = unsigned int(_boneId.y);
		m_BoneId[2] = unsigned int(_boneId.z);
		m_BoneId[3] = unsigned int(_boneId.w);
	}
};
