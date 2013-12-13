#include "ModelDefinition.h"

void ModelDefinition::getFinalTransform(float p_Time, std::vector<DirectX::XMFLOAT4X4> &ref)
{
	m_Time += p_Time;

	m_currentFrame += p_Time * 24.f;
	while (m_currentFrame >= (float)(m_Joints[0].m_JointAnimation.size() - 1))
	{
		m_currentFrame -= (float)(m_Joints[0].m_JointAnimation.size() - 1);
	}

	getAnimation();

	ref = m_FinalTransform;
	//return &m_FinalTransform;
}

void ModelDefinition::getAnimation()
{
	using namespace DirectX;
	const unsigned int numBones = m_Joints.size();
	
	m_FinalTransform.resize(numBones);
	std::vector<XMFLOAT4X4> toParentTranforms(numBones);

	for(unsigned int i = 0; i < m_Joints.size(); ++i)
	{
		m_Joints[i].interpolate(m_currentFrame, toParentTranforms[i]);
	}

	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	//toRootTransforms[0] = toParentTranforms[0];
	XMMATRIX localized = XMMatrixMultiply(XMLoadFloat4x4(&toParentTranforms[0]), XMLoadFloat4x4(&m_Joints[0].m_JointOffsetMatrix));
	XMStoreFloat4x4(&toRootTransforms[0], localized);

	for (unsigned int i = 1; i < numBones; i++)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTranforms[i]);
		toParent = XMMatrixMultiply(toParent, XMLoadFloat4x4(&m_Joints[i].m_JointOffsetMatrix));
		int parentIndex = m_Joints[i].m_Parent;
		parentIndex--;

		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);
		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);
		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	std::vector<XMFLOAT4X4> offsets(numBones);
	offsets[0] = m_Joints[0].m_JointOffsetMatrix;
	for (unsigned int i = 1; i < numBones; i++)
	{
		XMMATRIX parent = XMLoadFloat4x4(&offsets[m_Joints[i].m_Parent - 1]);
		XMMATRIX sumOffset = XMMatrixMultiply(XMLoadFloat4x4(&m_Joints[i].m_JointOffsetMatrix), parent);
		XMStoreFloat4x4(&offsets[i], sumOffset);
	}

	for (unsigned int i = 0; i < numBones; i++)
	{
		XMMATRIX offSet = XMLoadFloat4x4(&offsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMMATRIX identity = XMMatrixIdentity();
		
		offSet = XMMatrixInverse(nullptr, offSet);
		XMMATRIX result = XMMatrixTranspose(XMMatrixMultiply(offSet, toRoot));

		XMStoreFloat4x4(&m_FinalTransform[i], result);
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixTranspose(XMMatrixMultiply(offSet, toRoot)));
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixTranspose(XMMatrixMultiply(toRoot,offSet)));
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixMultiply(offSet, toRoot));
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixTranspose(XMMatrixMultiply(toRoot, offSet)));
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixTranspose(offSet));
		//XMStoreFloat4x4(&m_FinalTransform[i], offSet);
		//XMStoreFloat4x4(&m_FinalTransform[i], toRoot);
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixTranspose(toRoot));
		//XMStoreFloat4x4(&m_FinalTransform[i], identity);
	}
}