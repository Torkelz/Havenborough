#pragma once

#include <DirectXMath.h>
#include <vector>

struct KeyFrame
{
	DirectX::XMFLOAT3	m_Trans;
	DirectX::XMFLOAT4	m_Rot;
	DirectX::XMFLOAT3	m_Scale;
};

struct Joint
{
	std::string				m_JointName;
	int						m_ID;
	int						m_Parent;
	DirectX::XMFLOAT4X4		m_JointOffsetMatrix;
	std::vector<KeyFrame>	m_JointAnimation;

	void interpolate(float t, DirectX::XMFLOAT4X4& transform)
	{
		//float currentFrame = floor(t * 24);
		float currentFrame = t;
		//if (currentFrame < 1.0f)
		//	currentFrame = 1.0f;
		//if (currentFrame > 25.0f)
		//	currentFrame = 1.0f;

		KeyFrame temp = m_JointAnimation.at(unsigned int(currentFrame));

		DirectX::XMVECTOR scale			= DirectX::XMLoadFloat3(&temp.m_Scale);
		DirectX::XMVECTOR translation	= DirectX::XMLoadFloat3(&temp.m_Trans);

		DirectX::XMVECTOR rotation		= DirectX::XMLoadFloat4(&temp.m_Rot);

		DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixAffineTransformation(scale, zero, rotation, translation));
	}
};