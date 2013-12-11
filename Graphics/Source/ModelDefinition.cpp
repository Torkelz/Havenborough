#include "ModelDefinition.h"

void Model::getFinalTransform(float p_Time, std::vector<DirectX::XMFLOAT4X4> &ref)
{
	m_Time += p_Time;
	m_currentFrame++;
	if (m_currentFrame < 1.0f)
		m_currentFrame = 1.0f;
	if (m_currentFrame > 24.0f)
		m_currentFrame = 1.0f;
	getAnimation();

	ref = m_FinalTransform;
	//return &m_FinalTransform;
}

void Model::getAnimation()
{
	using namespace DirectX;
	unsigned int numBones = m_Joints.size();
	
	m_FinalTransform.resize(numBones);
	std::vector<XMFLOAT4X4> toParentTranforms(numBones);

	for(unsigned int i = 0; i < m_Joints.size(); ++i)
	{
		m_Joints[i].interpolate(m_currentFrame, toParentTranforms[i]);
	}

	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	toRootTransforms[0] = toParentTranforms[0];

	for (unsigned int i = 1; i < numBones; i++)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTranforms[i]);
		int parentIndex = m_Joints[i].m_Parent;
		parentIndex--;

		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);
		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);
		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	for (unsigned int i = 0; i < numBones; i++)
	{
		XMMATRIX offSet = XMLoadFloat4x4(&m_Joints[i].m_JointOffsetMatrix);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMMATRIX identity = XMMatrixIdentity();
		
		//offSet = XMMatrixInverse(&XMMatrixDeterminant(offSet), offSet);
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixMultiply(offSet, toRoot));
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixTranspose(XMMatrixMultiply(toRoot,offSet)));
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixMultiply(toRoot,offSet));
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixTranspose(offSet));
		//XMStoreFloat4x4(&m_FinalTransform[i], offSet);
		XMStoreFloat4x4(&m_FinalTransform[i], toRoot);
		//XMStoreFloat4x4(&m_FinalTransform[i], XMMatrixTranspose(toRoot));
	}
}