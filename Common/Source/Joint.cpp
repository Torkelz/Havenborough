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
	DirectX::XMVECTOR rotation1		= DirectX::XMLoadFloat4(&first.m_Rot) * interpolateFraction1;

	DirectX::XMVECTOR scale2		= DirectX::XMLoadFloat3(&second.m_Scale) * interpolateFraction2;
	DirectX::XMVECTOR translation2	= DirectX::XMLoadFloat3(&second.m_Trans) * interpolateFraction2;
	DirectX::XMVECTOR rotation2		= DirectX::XMLoadFloat4(&second.m_Rot) * interpolateFraction2;

	DirectX::XMFLOAT4X4 result;
	// Transpose after creation to get a row major matrix

	XMMATRIX invOffset = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_JointOffsetMatrix));
	XMMATRIX transMat = XMMatrixTranslationFromVector(XMVector3Transform(translation1 + translation2, invOffset));

	XMMATRIX scaleMat = XMMatrixScalingFromVector(scale1 + scale2);
	XMMATRIX rotMat = XMMatrixRotationQuaternion(rotation1 + rotation2);

	XMStoreFloat4x4(&result, XMMatrixTranspose(scaleMat * rotMat * transMat));

	return result;
}

MatrixDecomposed Joint::interpolateEx(float p_FrameTime, float m_DestinationFrameTime) const
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

	MatrixDecomposed result;

	XMMATRIX invOffset = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_JointOffsetMatrix));
	XMStoreFloat4(&result.translation, XMVector3Transform(translation1 + translation2, invOffset));
	XMStoreFloat4(&result.scale, scale1 + scale2);
	XMStoreFloat4(&result.rotation, rotation1 + rotation2);

	return result;
}

MatrixDecomposed Joint::interpolateEx(const MatrixDecomposed& p_Frame1, const MatrixDecomposed& p_Frame2, float interpolateFraction) const
{
	if(interpolateFraction > 1.0f)
		interpolateFraction = 1.0f;

	using namespace DirectX;
	float interpolateFractionInv = 1.f - interpolateFraction;

	XMVECTOR scale1			= XMLoadFloat4(&p_Frame1.scale);// * interpolateFractionInv;
	XMVECTOR translation1	= XMLoadFloat4(&p_Frame1.translation) * interpolateFractionInv;
	XMVECTOR rotation1		= XMLoadFloat4(&p_Frame1.rotation) * interpolateFractionInv;

	//XMVECTOR scale2			= XMLoadFloat4(&p_Frame2.scale) * interpolateFraction;
	XMVECTOR translation2	= XMLoadFloat4(&p_Frame2.translation) * interpolateFraction;
	XMVECTOR rotation2		= XMLoadFloat4(&p_Frame2.rotation) * interpolateFraction;

	MatrixDecomposed result;

	XMStoreFloat4(&result.translation, translation1 + translation2);
	XMStoreFloat4(&result.scale, scale1);
	XMStoreFloat4(&result.rotation, rotation1 + rotation2);

	return result;
}