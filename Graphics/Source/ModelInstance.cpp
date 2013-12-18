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

	m_LocalTransforms.resize(numBones);

	for(unsigned int i = 0; i < numBones; ++i)
	{
		XMFLOAT4X4 toParentData = p_Joints[i].interpolate(m_CurrentFrame);
		XMMATRIX toParent = XMMatrixMultiply(XMMatrixTranspose(XMLoadFloat4x4(&p_Joints[i].m_JointOffsetMatrix)), XMLoadFloat4x4(&toParentData));
		XMStoreFloat4x4(&m_LocalTransforms[i], toParent);
	}

	updateFinalTransforms(p_Joints);
}

const std::vector<DirectX::XMFLOAT4X4>& ModelInstance::getFinalTransform() const
{
	return m_FinalTransform;
}

void ModelInstance::applyIK_ReachPoint(const std::string& p_JointName, const DirectX::XMFLOAT3& p_Position, const std::vector<Joint>& p_Joints)
{
	using namespace DirectX;

	XMFLOAT4 fTarget(p_Position.x, p_Position.y, p_Position.z, 1.f);
	XMVECTOR target = XMLoadFloat4(&fTarget);

	const Joint* endJoint = nullptr;
	const Joint* middleJoint = nullptr;
	const Joint* baseJoint = nullptr;

	for (unsigned int i = 0; i < p_Joints.size(); i++)
	{
		const Joint& joint = p_Joints[i];

		if (joint.m_JointName == p_JointName)
		{
			endJoint = &joint;
		}
	}

	if (endJoint == nullptr || endJoint->m_Parent == 0)
	{
		return;
	}

	middleJoint = &p_Joints[endJoint->m_Parent - 1];

	if (middleJoint->m_Parent == 0)
	{
		return;
	}

	baseJoint = &p_Joints[middleJoint->m_Parent - 1];

	XMMATRIX world = XMLoadFloat4x4(&getWorldMatrix());

	XMMATRIX endCombinedTransform = XMMatrixMultiply(
		world,
		XMMatrixMultiply(
			XMLoadFloat4x4(&m_FinalTransform[endJoint->m_ID - 1]),
			XMLoadFloat4x4(&endJoint->m_TotalJointOffset)));
	XMMATRIX middleCombinedTransform = XMMatrixMultiply(
		world,
		XMMatrixMultiply(
			XMLoadFloat4x4(&m_FinalTransform[middleJoint->m_ID - 1]),
			XMLoadFloat4x4(&middleJoint->m_TotalJointOffset)));
	XMMATRIX baseCombinedTransform = XMMatrixMultiply(
		world,
		XMMatrixMultiply(
			XMLoadFloat4x4(&m_FinalTransform[baseJoint->m_ID - 1]),
			XMLoadFloat4x4(&baseJoint->m_TotalJointOffset)));

	XMFLOAT4X4 endCombinedTransformedData;
	XMFLOAT4X4 middleCombinedTransformedData;
	XMFLOAT4X4 baseCombinedTransformedData;

	XMStoreFloat4x4(&endCombinedTransformedData, endCombinedTransform);
	XMStoreFloat4x4(&middleCombinedTransformedData, middleCombinedTransform);
	XMStoreFloat4x4(&baseCombinedTransformedData, baseCombinedTransform);

	XMFLOAT4 startPosition(baseCombinedTransformedData._14, baseCombinedTransformedData._24, baseCombinedTransformedData._34, 1.f); 
	XMFLOAT4 jointPosition(middleCombinedTransformedData._14, middleCombinedTransformedData._24, middleCombinedTransformedData._34, 1.f); 
	XMFLOAT4 endPosition(endCombinedTransformedData._14, endCombinedTransformedData._24, endCombinedTransformedData._34, 1.f); 

	XMVECTOR startPositionV = XMLoadFloat4(&startPosition);
	XMVECTOR jointPositionV = XMLoadFloat4(&jointPosition);
	XMVECTOR endPositionV = XMLoadFloat4(&endPosition);

	XMVECTOR startToTarget = target - startPositionV;
	XMVECTOR startToJoint = jointPositionV - startPositionV;
	XMVECTOR jointToEnd = endPositionV - jointPositionV;

	float distStartToTarget = XMVector4Length(startToTarget).m128_f32[0];
	float distStartToJoint = XMVector4Length(startToJoint).m128_f32[0];
	float distJointToEnd = XMVector4Length(jointToEnd).m128_f32[0];

	float wantedJointAngle = 0.f;

	if (distStartToTarget >= distStartToJoint + distJointToEnd)
	{
		wantedJointAngle = XMConvertToRadians(180.f);
	}
	else
	{
		float cosAngle = (distStartToJoint * distStartToJoint
			+ distJointToEnd * distJointToEnd
			- distStartToTarget * distStartToTarget)
			/ (2.f * distStartToJoint * distJointToEnd);
		wantedJointAngle = XMScalarACos(cosAngle);
	}

	XMVECTOR nmlStartToJoint = XMVector4Normalize(startToJoint);
	XMVECTOR nmlJointToEnd = XMVector4Normalize(jointToEnd);

	float currentJointAngle = XMScalarACos(XMVector3Dot(-nmlStartToJoint, nmlJointToEnd).m128_f32[0]);

	float diffJointAngle = wantedJointAngle - currentJointAngle;
	static const XMFLOAT4 rotationAxisData(0.f, 0.f, 1.f, 0.f);
	XMVECTOR rotationAxis = XMLoadFloat4(&rotationAxisData);
	XMMATRIX rotation = XMMatrixRotationAxis(rotationAxis, diffJointAngle);

	XMStoreFloat4x4(&m_LocalTransforms[middleJoint->m_ID - 1], XMMatrixMultiply(XMLoadFloat4x4(&m_LocalTransforms[middleJoint->m_ID - 1]), rotation));

	XMFLOAT4X4 tempMatrixData = middleCombinedTransformedData;
	tempMatrixData._14 = 0.f;
	tempMatrixData._24 = 0.f;
	tempMatrixData._34 = 0.f;
	tempMatrixData._44 = 1.f;
	XMMATRIX tempMatrix = XMMatrixTranspose(XMLoadFloat4x4(&tempMatrixData));

	XMVECTOR worldHingeAxis = XMVector4Transform(rotationAxis, tempMatrix);
	worldHingeAxis = XMVector3Normalize(worldHingeAxis);
	rotation = XMMatrixRotationAxis(worldHingeAxis, diffJointAngle);
	XMVECTOR newJointToEnd = XMVector4Transform(jointToEnd, rotation);

	XMVECTOR newEndPosition = newJointToEnd + jointPositionV;

	XMMATRIX mtxToLocal = XMMatrixTranspose(XMMatrixInverse(nullptr, baseCombinedTransform));
	XMVECTOR localNewEnd = XMVector3Transform(newEndPosition, mtxToLocal);
	XMVECTOR localTarget = XMVector3TransformCoord(target, mtxToLocal);
	localNewEnd = XMVector3Normalize(localNewEnd);
	localTarget = XMVector3Normalize(localTarget);

	XMVECTOR localAxis = XMVector3Cross(localNewEnd, localTarget);
	if (XMVector3Length(localAxis).m128_f32[0] == 0.f)
	{
		return;
	}

	localAxis = XMVector3Normalize(localAxis);
	float localAngle = XMScalarACos(XMVector3Dot(localNewEnd, localTarget).m128_f32[0]);

	rotation = XMMatrixRotationAxis(localAxis, -localAngle);
	XMStoreFloat4x4(&m_LocalTransforms[baseJoint->m_ID - 1], XMMatrixMultiply(XMLoadFloat4x4(&m_LocalTransforms[baseJoint->m_ID - 1]), rotation));

	updateFinalTransforms(p_Joints);
}

DirectX::XMFLOAT3 ModelInstance::getJointPos(const std::string& p_JointName, const std::vector<Joint>& p_Joints)
{
	for (const auto& joint : p_Joints)
	{
		if (joint.m_JointName == p_JointName)
		{
				XMMATRIX jointCombinedTransform = XMMatrixMultiply(
					XMLoadFloat4x4(&getWorldMatrix()),
					XMMatrixMultiply(
						XMLoadFloat4x4(&m_FinalTransform[joint.m_ID - 1]),
						XMLoadFloat4x4(&joint.m_TotalJointOffset)));

				XMFLOAT4X4 jointCombinedTransformData;
				XMStoreFloat4x4(&jointCombinedTransformData, jointCombinedTransform);

				XMFLOAT3 jointPosition(jointCombinedTransformData._14, jointCombinedTransformData._24, jointCombinedTransformData._34); 

				return jointPosition;
		}
	}

	return XMFLOAT3(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
}

void ModelInstance::updateFinalTransforms(const std::vector<Joint>& p_Joints)
{
	unsigned int numBones = p_Joints.size();

	std::vector<XMFLOAT4X4> toRootTransforms(numBones);
	toRootTransforms[0] = m_LocalTransforms[0];

	for (unsigned int i = 1; i < numBones; i++)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&m_LocalTransforms[i]);
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[p_Joints[i].m_Parent - 1]);

		XMMATRIX toRoot = XMMatrixMultiply(parentToRoot, toParent);
		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	static const XMFLOAT4X4 flipMatrixData(
		-1.f, 0.f, 0.f, 0.f,
		 0.f, 1.f, 0.f, 0.f,
		 0.f, 0.f, 1.f, 0.f,
		 0.f, 0.f, 0.f, 1.f);

	static const XMMATRIX flipMatrix = XMLoadFloat4x4(&flipMatrixData);
	
	m_FinalTransform.resize(numBones);

	for (unsigned int i = 0; i < numBones; i++)
	{
		XMMATRIX offSet = XMLoadFloat4x4(&p_Joints[i].m_TotalJointOffset);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		
		offSet = XMMatrixInverse(nullptr, offSet);
		XMMATRIX result = XMMatrixMultiply(toRoot, offSet);

		result = XMMatrixMultiply(flipMatrix, result);

		XMStoreFloat4x4(&m_FinalTransform[i], result);
	}
}
