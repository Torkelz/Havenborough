#include "ModelInstance.h"

using namespace DirectX;
using std::string;

ModelInstance::ModelInstance()
	:	m_CurrentFrame(0.f)
{
}

ModelInstance::~ModelInstance()
{
}

void ModelInstance::setIsCalculated(bool p_IsCalculated)
{
	m_IsCalculated = p_IsCalculated;
}

bool ModelInstance::getIsCalculated(void) const
{
	return m_IsCalculated;
}

void ModelInstance::setModelName(const string& p_Name)
{
	m_ModelName = p_Name;
}

string ModelInstance::getModelName(void) const
{
	return m_ModelName;
}

const XMFLOAT4X4& ModelInstance::getWorldMatrix() const
{
	if (!m_IsCalculated)
	{
		calculateWorldMatrix();
	}
	return m_World;
}

void ModelInstance::setPosition(const XMFLOAT3& p_Position)
{
	m_Position = p_Position;
	m_IsCalculated = false;
}

void ModelInstance::setRotation(const XMFLOAT3& p_Rotation)
{
	m_Rotation = p_Rotation;
	m_IsCalculated = false;
}

void ModelInstance::setScale(const XMFLOAT3& p_Scale)
{
	m_Scale = p_Scale;
	m_IsCalculated = false;
}

void ModelInstance::calculateWorldMatrix() const
{
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Rotation));
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position));
	XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale));

	XMStoreFloat4x4(&m_World, XMMatrixTranspose(scale * rotation * translation));

	m_IsCalculated = true;
}

void ModelInstance::updateAnimation(float p_DeltaTime, const std::vector<Joint>& p_Joints)
{
	m_CurrentFrame += p_DeltaTime * 24.f;
	while (m_CurrentFrame >= (float)(p_Joints[0].m_JointAnimation.size() - 1))
	{
		m_CurrentFrame -= (float)(p_Joints[0].m_JointAnimation.size() - 1);
	}

	using namespace DirectX;
	const unsigned int numBones = p_Joints.size();
	
	m_FinalTransform.resize(numBones);
	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	for(unsigned int i = 0; i < p_Joints.size(); ++i)
	{
		toParentTransforms[i] = p_Joints[i].interpolate(m_CurrentFrame);
	}

	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	XMMATRIX localized = XMMatrixMultiply(XMLoadFloat4x4(&toParentTransforms[0]), XMLoadFloat4x4(&p_Joints[0].m_JointOffsetMatrix));
	XMStoreFloat4x4(&toRootTransforms[0], localized);

	for (unsigned int i = 1; i < numBones; i++)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);
		toParent = XMMatrixMultiply(toParent, XMLoadFloat4x4(&p_Joints[i].m_JointOffsetMatrix));
		int parentIndex = p_Joints[i].m_Parent;
		parentIndex--;

		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);
		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);
		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	std::vector<XMFLOAT4X4> offsets(numBones);
	offsets[0] = p_Joints[0].m_JointOffsetMatrix;
	for (unsigned int i = 1; i < numBones; i++)
	{
		XMMATRIX parent = XMLoadFloat4x4(&offsets[p_Joints[i].m_Parent - 1]);
		XMMATRIX sumOffset = XMMatrixMultiply(XMLoadFloat4x4(&p_Joints[i].m_JointOffsetMatrix), parent);
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
	}
}

const std::vector<DirectX::XMFLOAT4X4>& ModelInstance::getFinalTransform() const
{
	return m_FinalTransform;
}
