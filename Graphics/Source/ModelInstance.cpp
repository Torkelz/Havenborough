#include "ModelInstance.h"
#include "GraphicsExceptions.h"
#include "GraphicsLogger.h"

using namespace DirectX;
using std::string;
using std::vector;

ModelInstance::ModelInstance()
	: m_IsCalculated(false), 
	m_ColorTone(DirectX::XMFLOAT3(1.f, 1.f, 1.f))
{
	for (int i = 0; i < 6; i++)
	{
		m_Tracks[i].active = false;
		m_Tracks[i].fadedFrames = 0.0f;
		m_Tracks[i].currentFrame = 0.0f;
		m_Tracks[i].destinationFrame = 1.0f;
		m_Tracks[i].dynamicWeight = 1.0f;
		m_Tracks[i].clip = AnimationClip();
	}
}

ModelInstance::~ModelInstance() {}

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

void ModelInstance::setColorTone(const XMFLOAT3& p_ColorTone)
{
	m_ColorTone = p_ColorTone;
}

const XMFLOAT3 &ModelInstance::getColorTone() const
{
	return m_ColorTone;
}

void ModelInstance::calculateWorldMatrix() const
{
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Rotation));
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position));
	XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale));

	XMStoreFloat4x4(&m_World, XMMatrixTranspose(scale * rotation * translation));

	m_IsCalculated = true;
}

void ModelInstance::updateAnimation(float p_DeltaTime, const vector<Joint>& p_Joints)
{
	// Update time stamp in the direction of the animation speed per track.
	updateTimeStamp(p_DeltaTime);

	// Check if any fade blends are active and if they should end.
	checkFades();

	const unsigned int numBones = p_Joints.size();

	m_LocalTransforms.resize(numBones);

	// Calculate the local transformations for each joint. Has 
	// to be done before IK is calculated and applied.
	for(unsigned int i = 0; i < numBones; ++i)
	{
		MatrixDecomposed toParentData;
		// Track 0 is the main track. Do vanilla animation.
		if(m_Tracks[0].clip.m_AnimationSpeed > 0)
		{
			toParentData = p_Joints[i].interpolateEx(m_Tracks[0].currentFrame, m_Tracks[0].destinationFrame);
		}
		else
		{
			toParentData = p_Joints[i].interpolateEx(m_Tracks[0].destinationFrame, m_Tracks[0].currentFrame);
		}

		for(unsigned int currentTrack = 1; currentTrack < 6; currentTrack++)
		{
			if (m_Tracks[currentTrack].active)
			{
				if (currentTrack > 3)
				{
					if (affected(p_Joints, i, m_Tracks[currentTrack].clip.m_FirstJoint))
					{
						toParentData = updateKeyFrameInformation(p_Joints[i], currentTrack, toParentData);
					}
				}
				else
					toParentData = updateKeyFrameInformation(p_Joints[i], currentTrack, toParentData);
			}
		}

		if (m_Tracks[4].active)
		{
			XMVECTOR temp = XMLoadFloat4(&toParentData.translation);
			float leng = XMVectorGetX(XMVector3Length(temp));
			if(leng > 0.0001f)
				int lollzzz = 0;

			temp = XMLoadFloat4(&toParentData.scale);
			leng = XMVectorGetX(XMVector3Length(temp));
			if(leng > 1.733f)
				int lollzzz = 0;
		}

		XMMATRIX transMat = XMMatrixTranslationFromVector(XMLoadFloat4(&toParentData.translation));
		XMMATRIX scaleMat = XMMatrixScalingFromVector(XMLoadFloat4(&toParentData.scale));
		XMMATRIX rotMat = XMMatrixRotationQuaternion(XMLoadFloat4(&toParentData.rotation));
		XMMATRIX toParentMatrix = XMMatrixTranspose(scaleMat * rotMat * transMat);

		XMMATRIX toParent = XMMatrixMultiply(XMMatrixTranspose(XMLoadFloat4x4(&p_Joints[i].m_JointOffsetMatrix)),
			toParentMatrix);
		XMStoreFloat4x4(&m_LocalTransforms[i], toParent);
	}

	updateFinalTransforms(p_Joints);
}

MatrixDecomposed ModelInstance::updateKeyFrameInformation(Joint p_Joint, unsigned int p_CurrentTrack,
	MatrixDecomposed p_ToParentData)
{
	MatrixDecomposed tempData;
	if(m_Tracks[p_CurrentTrack].clip.m_AnimationSpeed > 0)
	{
		tempData = p_Joint.interpolateEx(m_Tracks[p_CurrentTrack].currentFrame, m_Tracks[p_CurrentTrack].destinationFrame);
	}
	else
	{
		tempData = p_Joint.interpolateEx(m_Tracks[p_CurrentTrack].destinationFrame, m_Tracks[p_CurrentTrack].currentFrame);
	}

	if(m_Tracks[p_CurrentTrack].clip.m_FadeIn)
	{
		return p_Joint.interpolateEx(p_ToParentData, tempData, (m_Tracks[p_CurrentTrack].fadedFrames /
			(float)m_Tracks[p_CurrentTrack].clip.m_FadeInFrames) * m_Tracks[p_CurrentTrack].clip.m_Weight *
			m_Tracks[p_CurrentTrack].dynamicWeight);
	}
	else if(m_Tracks[p_CurrentTrack].clip.m_FadeOut && !m_Tracks[p_CurrentTrack].clip.m_Loop)
	{
		return p_Joint.interpolateEx(p_ToParentData, tempData, 1.0f - ((m_Tracks[p_CurrentTrack].fadedFrames /
			(float)m_Tracks[p_CurrentTrack].clip.m_FadeOutFrames) * m_Tracks[p_CurrentTrack].clip.m_Weight) *
			m_Tracks[p_CurrentTrack].dynamicWeight );
	}
	else
	{
		return p_Joint.interpolateEx(p_ToParentData, tempData, m_Tracks[p_CurrentTrack].clip.m_Weight *
			m_Tracks[p_CurrentTrack].dynamicWeight );
	}
}

void ModelInstance::checkFades()
{
	for (int i = 1; i < 6; i++)
	{
		if (m_Tracks[i].clip.m_FadeIn)
		{
			if ((float)m_Tracks[i].clip.m_FadeInFrames <= m_Tracks[i].fadedFrames)
			{
				m_Tracks[i].clip.m_FadeIn = false;
				m_Tracks[i].fadedFrames = 0.0f;

				if(!m_Tracks[i].clip.m_Layered && m_Tracks[i].clip.m_Loop)
				{
					m_Tracks[i - 1] = m_Tracks[i];
					m_Tracks[i].active = false;
					m_Tracks[i - 1].active = true;
				}
			}
		}
		if (!m_Tracks[i].clip.m_FadeIn && m_Tracks[i].clip.m_FadeOut)
		{
			if ((float)m_Tracks[i].clip.m_FadeOutFrames <= m_Tracks[i].fadedFrames)
			{
				m_Tracks[i].clip.m_FadeOut = false;
				m_Tracks[i].fadedFrames = 0.0f;
				m_Tracks[i].active = false;
			}
		}
		if( (float)(m_Tracks[i].clip.m_End - m_Tracks[i].clip.m_FadeOutFrames - 1) < m_Tracks[i].currentFrame)
		{
			if(!playQueuedClip(i))
			{
				m_Tracks[i].active = false;
			}
		}
	}
}

void ModelInstance::updateTimeStamp(float p_DeltaTime)
{
	static const float keyfps = 24.0f;

	for (int i = 0; i < 6; i++)
	{
		if( m_Tracks[i].active )
		{
			float frameStep = p_DeltaTime * keyfps * m_Tracks[i].clip.m_AnimationSpeed;
			m_Tracks[i].currentFrame += frameStep;

			if (m_Tracks[i].clip.m_FadeIn)
				m_Tracks[i].fadedFrames += abs(frameStep);
			else if ( m_Tracks[i].clip.m_FadeOut && !m_Tracks[i].clip.m_Loop)
			{
				if( (float)(m_Tracks[i].clip.m_End - m_Tracks[i].clip.m_FadeOutFrames - 1) <= m_Tracks[i].currentFrame)
					m_Tracks[i].fadedFrames += abs(frameStep);
			}

			const unsigned int numKeyframes = (unsigned int)m_Tracks[i].clip.m_End;
	
			if (m_Tracks[i].clip.m_AnimationSpeed > 0.0f)
			{
				m_Tracks[i].destinationFrame = m_Tracks[i].currentFrame + 1.0f;
				if (m_Tracks[i].currentFrame >= (float)(numKeyframes))
				{
					if(m_Tracks[i].clip.m_Loop)
						m_Tracks[i].currentFrame = (float)m_Tracks[i].clip.m_Start;
					else
						m_Tracks[i].active = false;
				}
				if (m_Tracks[i].destinationFrame >= (float)(numKeyframes))
				{
					if(m_Tracks[i].clip.m_Loop)
					{
						float dummy;
						float frac = modff(m_Tracks[i].destinationFrame, &dummy);
						m_Tracks[i].destinationFrame = m_Tracks[i].clip.m_Start + frac;
					}
					else
						m_Tracks[i].active = false;
				}
			}
			else
			{
				if (m_Tracks[i].currentFrame <= m_Tracks[i].clip.m_Start)
				{
					if(m_Tracks[i].clip.m_Loop)
						m_Tracks[i].currentFrame = (float)(numKeyframes);
					else
						m_Tracks[i].active = false;
				}
				m_Tracks[i].destinationFrame = m_Tracks[i].currentFrame - 1.0f;
				if (m_Tracks[i].destinationFrame <= m_Tracks[i].clip.m_Start)
				{
					if(m_Tracks[i].clip.m_Loop)
					{
						float frac = 0.0f;
						frac = m_Tracks[i].destinationFrame - (int)m_Tracks[i].destinationFrame;
						m_Tracks[i].destinationFrame = (float)(numKeyframes) + frac - 1;
					}
					else
						m_Tracks[i].active = false;
				}
			}
		}
	}
}

bool ModelInstance::affected(const vector<Joint>& p_Joints, int p_ID, string p_FirstAffectedJoint)
{
	// FirstAffectedJode = FAJ
	if (p_Joints[p_ID].m_JointName == p_FirstAffectedJoint) // The FAJ has been found.
		return true;
	else if(p_ID == 0) // The root joint has been reached and the FAJ has not been found.
		return false;
	else return affected(p_Joints, p_Joints.at(p_ID).m_Parent - 1, p_FirstAffectedJoint); // Neither the root nor the FAJ has been found yet, keep looking.
}

const vector<DirectX::XMFLOAT4X4>& ModelInstance::getFinalTransform() const
{
	return m_FinalTransform;
}

void ModelInstance::applyIK_ReachPoint(const IKGroup& p_Group, const DirectX::XMFLOAT3& p_Position,
		const std::vector<Joint>& p_Joints)
{
	XMFLOAT4 targetData(p_Position.x, p_Position.y, p_Position.z, 1.f);
	XMVECTOR target = XMLoadFloat4(&targetData);

	// The algorithm uses three joints, one end joint that wants to reach a target (point).
	// This joint is not tranformed in it self. The middle joint works like a human elbow and
	// has only 1 DoF. It will make sure that the "arm" is bent if the target point is closer 
	// to the base joint than the length of the "arm" when fully extended. The base joint works
	// like a human shoulder it has 3 DoF and will make sure that the "arm" is always pointed
	// towards the target point.
	const Joint* endJoint = nullptr;
	const Joint* middleJoint = nullptr;
	const Joint* baseJoint = nullptr;

	for (unsigned int i = 0; i < p_Joints.size(); i++)
	{
		const Joint& joint = p_Joints[i];

		if (joint.m_JointName == p_Group.m_Hand)
		{
			endJoint = &joint;
		}
		else if (joint.m_JointName == p_Group.m_Elbow)
		{
			middleJoint = &joint;
		}
		else if (joint.m_JointName == p_Group.m_Shoulder)
		{
			baseJoint = &joint;
		}
	}

	// The algorithm requires all three joints
	if (endJoint == nullptr || middleJoint == nullptr || baseJoint == nullptr)
	{
		return;
	}

	XMMATRIX world = XMLoadFloat4x4(&getWorldMatrix());

	// Calculate matrices for transforming vectors from joint spaces to world space
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

	// The joints' positions in world space is the zero vector in joint space transformed to world space.
	XMFLOAT4 startPosition(baseCombinedTransformedData._14, baseCombinedTransformedData._24,
		baseCombinedTransformedData._34, 1.f); 
	XMFLOAT4 jointPosition(middleCombinedTransformedData._14, middleCombinedTransformedData._24,
		middleCombinedTransformedData._34, 1.f); 
	XMFLOAT4 endPosition(endCombinedTransformedData._14, endCombinedTransformedData._24,
		endCombinedTransformedData._34, 1.f); 

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

	// Fully extend the joints when target not reachable
	if (distStartToTarget >= distStartToJoint + distJointToEnd)
	{
		wantedJointAngle = XMConvertToRadians(180.f);
	}
	else
	{
		// Smallest angle using the law of cosine
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

	// Asume all "elbows" has the positive Z axis as hinge axis.
	//static const XMFLOAT4 rotationAxisData(0.f, -1.f, 0.f, 0.f);
	XMVECTOR rotationAxis = XMLoadFloat3(&p_Group.m_ElbowHingeAxis);//XMLoadFloat4(&rotationAxisData);
	XMMATRIX rotation = XMMatrixRotationAxis(rotationAxis, diffJointAngle);

	// Rotate the local transform of the "elbow" joint
	XMStoreFloat4x4(&m_LocalTransforms[middleJoint->m_ID - 1],
		XMMatrixMultiply(XMLoadFloat4x4(&m_LocalTransforms[middleJoint->m_ID - 1]), rotation));

	XMFLOAT4X4 tempMatrixData = middleCombinedTransformedData;
	XMMATRIX tempMatrix = XMMatrixTranspose(XMLoadFloat4x4(&tempMatrixData));

	// Move the end joint in world space
	XMVECTOR worldHingeAxis = XMVector4Transform(rotationAxis, tempMatrix);
	rotation = XMMatrixRotationAxis(worldHingeAxis, diffJointAngle);
	XMVECTOR newJointToEnd = XMVector4Transform(jointToEnd, rotation);

	XMVECTOR newEndPosition = newJointToEnd + jointPositionV;

	// Transform positions to the joint space of the "shoulder"
	XMMATRIX mtxToLocal = XMMatrixTranspose(XMMatrixInverse(nullptr, baseCombinedTransform));
	XMVECTOR localNewEnd = XMVector3Transform(newEndPosition, mtxToLocal);
	XMVECTOR localTarget = XMVector3TransformCoord(target, mtxToLocal);
	localNewEnd = XMVector3Normalize(localNewEnd);
	localTarget = XMVector3Normalize(localTarget);

	// Calculate the axis for the shortest rotation
	XMVECTOR localAxis = XMVector3Cross(localNewEnd, localTarget);
	if (XMVector3Length(localAxis).m128_f32[0] == 0.f)
	{
		return;
	}

	localAxis = XMVector3Normalize(localAxis);
	float localAngle = XMScalarACos(XMVector3Dot(localNewEnd, localTarget).m128_f32[0]);

	// Rotate the local transform of the "shoudler" joint
	rotation = XMMatrixRotationAxis(localAxis, -localAngle);
	XMStoreFloat4x4(&m_LocalTransforms[baseJoint->m_ID - 1],
		XMMatrixMultiply(XMLoadFloat4x4(&m_LocalTransforms[baseJoint->m_ID - 1]), rotation));

	// Update the resulting child transformations
	updateFinalTransforms(p_Joints);
}

DirectX::XMFLOAT3 ModelInstance::getJointPos(const string& p_JointName, const vector<Joint>& p_Joints)
{
	for (const auto& joint : p_Joints)
	{
		if (joint.m_JointName == p_JointName)
		{
			// The joints' positions in world space is the zero vector in joint space transformed to world space.
			XMMATRIX jointCombinedTransform = XMMatrixMultiply(
				XMLoadFloat4x4(&getWorldMatrix()),
				XMMatrixMultiply(
					XMLoadFloat4x4(&m_FinalTransform[joint.m_ID - 1]),
					XMLoadFloat4x4(&joint.m_TotalJointOffset)));

			XMFLOAT4X4 jointCombinedTransformData;
			XMStoreFloat4x4(&jointCombinedTransformData, jointCombinedTransform);

			XMFLOAT3 jointPosition(jointCombinedTransformData._14, jointCombinedTransformData._24,
				jointCombinedTransformData._34); 

			return jointPosition;
		}
	}

	throw InvalidArgumentGraphicsException("Joint does not exist: '" + p_JointName + "'", __LINE__, __FILE__);
}

void ModelInstance::updateFinalTransforms(const vector<Joint>& p_Joints)
{
	const unsigned int numBones = p_Joints.size();

	vector<XMFLOAT4X4> toRootTransforms(numBones);

	// Accumulate parent transformations
	toRootTransforms[0] = m_LocalTransforms[0];
	for (unsigned int i = 1; i < numBones; i++)
	{
		const XMMATRIX toParent = XMLoadFloat4x4(&m_LocalTransforms[i]);
		const XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[p_Joints[i].m_Parent - 1]);

		const XMMATRIX toRoot = XMMatrixMultiply(parentToRoot, toParent);
		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	// Flip the X-axis to solve right-to-left-handed conversion
	static const XMFLOAT4X4 flipMatrixData(
		-1.f, 0.f, 0.f, 0.f,
		 0.f, 1.f, 0.f, 0.f,
		 0.f, 0.f, 1.f, 0.f,
		 0.f, 0.f, 0.f, 1.f);
	static const XMMATRIX flipMatrix = XMLoadFloat4x4(&flipMatrixData);
	
	m_FinalTransform.resize(numBones);

	// Use offset to account for bind space coordinates of vertex positions
	for (unsigned int i = 0; i < numBones; i++)
	{
		XMMATRIX offSet = XMLoadFloat4x4(&p_Joints[i].m_TotalJointOffset);
		const XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		
		offSet = XMMatrixInverse(nullptr, offSet);
		XMMATRIX result = XMMatrixMultiply(toRoot, offSet);

		result = XMMatrixMultiply(flipMatrix, result);

		XMStoreFloat4x4(&m_FinalTransform[i], result);

		// *** Save for debugging of new animated objects. ***
		//XMMATRIX identity = XMMatrixIdentity();
		//XMStoreFloat4x4(&m_FinalTransform[i], identity);
		//XMStoreFloat4x4(&m_FinalTransform[i], offSet);
	}
}

void ModelInstance::playClip( AnimationClip p_Clip, bool p_Override )
{
	int track = p_Clip.m_DestinationTrack;
	if(p_Override)
	{
		m_Tracks[track].clip = p_Clip;
		m_Tracks[track].fadedFrames = 0.0f;
		m_Tracks[track].active = true;
		m_Tracks[track].currentFrame = (float)p_Clip.m_Start;
		m_Tracks[track].dynamicWeight = 1.0f;
	}
	else
	{
		if(m_Tracks[track].active)
		{
			if(!m_Tracks[track + 1].active)
			{
				m_Tracks[track + 1].clip = p_Clip;
				m_Tracks[track + 1].fadedFrames = 0.0f;
				m_Tracks[track + 1].active = true;
				m_Tracks[track + 1].currentFrame = (float)p_Clip.m_Start;
				m_Tracks[track + 1].dynamicWeight = 1.0f;
			}
			else
			{	
				m_Tracks[track].clip			= 	m_Tracks[track + 1].clip;
				m_Tracks[track].fadedFrames		= 	m_Tracks[track + 1].fadedFrames;
				m_Tracks[track].active			= 	m_Tracks[track + 1].active;
				m_Tracks[track].currentFrame	= 	m_Tracks[track + 1].currentFrame;
				m_Tracks[track].dynamicWeight	= 	m_Tracks[track + 1].dynamicWeight;

				m_Tracks[track + 1].clip = p_Clip;
				m_Tracks[track + 1].fadedFrames = 0.0f;
				m_Tracks[track + 1].active = true;
				m_Tracks[track + 1].currentFrame = (float)p_Clip.m_Start;
				m_Tracks[track + 1].dynamicWeight = 1.0f;
			}
		}
		else
		{
			m_Tracks[track].clip = p_Clip;
			m_Tracks[track].fadedFrames = 0.0f;
			m_Tracks[track].active = true;
			m_Tracks[track].currentFrame = (float)p_Clip.m_Start;
			m_Tracks[track].dynamicWeight = 1.0f;
		}
	}
}

void ModelInstance::queueClip( AnimationClip p_Clip )
{
	m_Queue.push_back(p_Clip);
}

bool ModelInstance::playQueuedClip(int p_Track)
{
	if (p_Track == 1 || p_Track == 3 || p_Track == 5)
		p_Track--;

	if(!m_Queue.empty())
	{
		for (unsigned int i = 0; i < m_Queue.size(); i++)
		{
			if (m_Queue[i].m_DestinationTrack == p_Track)
			{
				playClip(m_Queue[i], false);
				m_Queue.erase(m_Queue.begin() + i);
				return true;
			}
		}
	}
	return false;
}

void ModelInstance::changeWeight(int p_Track, float p_Weight)
{
	if(p_Track > 0 && p_Track < 6)	
		m_Tracks[p_Track].dynamicWeight = m_Tracks[p_Track + 1].dynamicWeight = p_Weight;
}