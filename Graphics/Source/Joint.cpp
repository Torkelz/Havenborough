#include "Joint.h"

void Joint::interpolate(float t, DirectX::XMFLOAT4X4& transform)
{
	using namespace DirectX;

	float currentFrame = t;

	KeyFrame first = m_JointAnimation.at(unsigned int(currentFrame));
	KeyFrame second = m_JointAnimation.at((unsigned int)currentFrame + 1);

	float dummy;
	float interpolateFraction2 = modff(currentFrame, &dummy);
	float interpolateFraction1 = 1.f - interpolateFraction2;

	DirectX::XMVECTOR scale1		= DirectX::XMLoadFloat3(&first.m_Scale) * interpolateFraction1;
	DirectX::XMVECTOR translation1	= DirectX::XMLoadFloat3(&first.m_Trans) * interpolateFraction1;
	translation1 -= XMLoadFloat4x4(&m_JointOffsetMatrix).r[3] * interpolateFraction1;
	DirectX::XMVECTOR rotation1		= DirectX::XMLoadFloat4(&first.m_Rot) * interpolateFraction1;

	DirectX::XMVECTOR scale2		= DirectX::XMLoadFloat3(&second.m_Scale) * interpolateFraction2;
	DirectX::XMVECTOR translation2	= DirectX::XMLoadFloat3(&second.m_Trans) * interpolateFraction2;
	translation2 -= XMLoadFloat4x4(&m_JointOffsetMatrix).r[3] * interpolateFraction2;
	DirectX::XMVECTOR rotation2		= DirectX::XMLoadFloat4(&second.m_Rot) * interpolateFraction2;

	DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixAffineTransformation(scale1 + scale2, zero, rotation1 + rotation2, translation1 + translation2));
}
