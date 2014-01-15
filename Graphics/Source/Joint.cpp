#include "Joint.h"

DirectX::XMFLOAT4X4 Joint::interpolate(float p_FrameTime, float m_DestinationFrameTime) const
{
	using namespace DirectX;

	KeyFrame first = m_JointAnimation.at(unsigned int(p_FrameTime));
	KeyFrame second = m_JointAnimation.at((unsigned int)m_DestinationFrameTime);


	float dummy;
	float interpolateFraction2 = modff(p_FrameTime, &dummy);
	float interpolateFraction1 = 1.f - interpolateFraction2;

	DirectX::XMVECTOR scale1		= DirectX::XMLoadFloat3(&first.m_Scale) * interpolateFraction1;
	DirectX::XMVECTOR translation1	= DirectX::XMLoadFloat3(&first.m_Trans) * interpolateFraction1;
	// Account for Maya
	//translation1 -= XMLoadFloat4x4(&m_JointOffsetMatrix).r[3] * interpolateFraction1;
	DirectX::XMVECTOR rotation1		= DirectX::XMLoadFloat4(&first.m_Rot) * interpolateFraction1;

	DirectX::XMVECTOR scale2		= DirectX::XMLoadFloat3(&second.m_Scale) * interpolateFraction2;
	DirectX::XMVECTOR translation2	= DirectX::XMLoadFloat3(&second.m_Trans) * interpolateFraction2;
	// Account for Maya
	//translation2 -= XMLoadFloat4x4(&m_JointOffsetMatrix).r[3] * interpolateFraction2;
	DirectX::XMVECTOR rotation2		= DirectX::XMLoadFloat4(&second.m_Rot) * interpolateFraction2;

	DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	DirectX::XMFLOAT4X4 result;
	// Transpose after creation to get a row major matrix

	XMMATRIX invOffset = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_JointOffsetMatrix));
	XMMATRIX transMat = XMMatrixTranslationFromVector(XMVector3Transform(translation1 + translation2, invOffset));

	XMMATRIX scaleMat = XMMatrixScalingFromVector(scale1 + scale2);
	XMMATRIX rotMat = XMMatrixRotationQuaternion(rotation1 + rotation2);

	//DirectX::XMStoreFloat4x4(&result, XMMatrixTranspose(XMMatrixAffineTransformation(scale1 + scale2, zero, rotation1 + rotation2, translation1 + translation2)));
	XMStoreFloat4x4(&result, XMMatrixTranspose(scaleMat * rotMat * transMat));

	return result;
}

matrixDecomposed Joint::interpolateEx(float p_FrameTime, float m_DestinationFrameTime) const
{
	using namespace DirectX;

	KeyFrame first = m_JointAnimation.at(unsigned int(p_FrameTime));
	KeyFrame second = m_JointAnimation.at((unsigned int)m_DestinationFrameTime);

	float dummy;
	float interpolateFraction2 = modff(p_FrameTime, &dummy);
	float interpolateFraction1 = 1.f - interpolateFraction2;

	XMVECTOR scale1			= XMLoadFloat3(&first.m_Scale) * interpolateFraction1;
	XMVECTOR translation1	= XMLoadFloat3(&first.m_Trans) * interpolateFraction1;
	XMVECTOR rotation1		= XMLoadFloat4(&first.m_Rot) * interpolateFraction1;

	XMVECTOR scale2			= XMLoadFloat3(&second.m_Scale) * interpolateFraction2;
	XMVECTOR translation2	= XMLoadFloat3(&second.m_Trans) * interpolateFraction2;
	XMVECTOR rotation2		= XMLoadFloat4(&second.m_Rot) * interpolateFraction2;

	XMVECTOR zero			= XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	matrixDecomposed result;

	XMMATRIX invOffset = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_JointOffsetMatrix));
	XMStoreFloat4(&result.translation, XMVector3Transform(translation1 + translation2, invOffset));
	XMStoreFloat4(&result.scale, scale1 + scale2);
	XMStoreFloat4(&result.rotation, rotation1 + rotation2);

	return result;
}

matrixDecomposed Joint::interpolateEx(matrixDecomposed p_Frame1, matrixDecomposed p_Frame2, float interpolateFraction) const
{
	using namespace DirectX;
	
	XMVECTOR scale1			= XMLoadFloat4(&p_Frame1.scale);
	XMVECTOR translation1	= XMLoadFloat4(&p_Frame1.translation);
	XMVECTOR rotation1		= XMLoadFloat4(&p_Frame1.rotation);

	XMVECTOR scale2			= XMLoadFloat4(&p_Frame2.scale);
	XMVECTOR translation2	= XMLoadFloat4(&p_Frame2.translation);
	XMVECTOR rotation2		= XMLoadFloat4(&p_Frame2.rotation);

	XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	matrixDecomposed result;

	XMStoreFloat4(&result.translation, (translation1 * (1.f - interpolateFraction)) + (translation2 * interpolateFraction));
	XMStoreFloat4(&result.scale, (scale1 * (1.f - interpolateFraction)) + (scale2 * interpolateFraction));
	XMStoreFloat4(&result.rotation, (rotation1 * (1.f - interpolateFraction)) + (rotation2 * interpolateFraction));

	return result;
}