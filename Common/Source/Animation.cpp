#include "Animation.h"
#include "CommonExceptions.h"
#include "Logger.h"

#include <algorithm>

using namespace DirectX;
using std::string;
using std::vector;

Animation::Animation()
{
	for (int i = 0; i < 6; i++)
	{
		m_Tracks[i].active = false;
		m_Tracks[i].fadedFrames = 0.0f;
		m_Tracks[i].currentFrame = 0.0f;
		m_Tracks[i].destinationFrame = 1.0f;
		m_Tracks[i].dynamicWeight = 1.0f;
		m_Tracks[i].clip = nullptr;
		m_Tracks[i].fadeIn = false;
		m_Tracks[i].fadeOut = false;
	}
}

Animation::~Animation() {}

void Animation::updateAnimation(float p_DeltaTime)
{
	const std::vector<Joint>& p_Joints = m_Data->joints;

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
		if(m_Tracks[0].clip->m_AnimationSpeed > 0)
		{
			toParentData = std::move(p_Joints[i].interpolateEx(m_Tracks[0].currentFrame, m_Tracks[0].destinationFrame));
		}
		else
		{
			toParentData = std::move(p_Joints[i].interpolateEx(m_Tracks[0].destinationFrame, m_Tracks[0].currentFrame));
		}

		for(unsigned int currentTrack = 1; currentTrack < 6; currentTrack++)
		{
			if (m_Tracks[currentTrack].active)
			{
				if (currentTrack > 3)
				{
					if (affected(i, m_Tracks[currentTrack].clip->m_FirstJoint))
					{
						toParentData = updateKeyFrameInformation(p_Joints[i], currentTrack, toParentData);
					}
				}
				else
					toParentData = updateKeyFrameInformation(p_Joints[i], currentTrack, toParentData);
			}
		}

		XMMATRIX transMat = XMMatrixTranslationFromVector(XMLoadFloat4(&toParentData.translation));
		XMMATRIX scaleMat = XMMatrixScalingFromVector(XMLoadFloat4(&toParentData.scale));
		XMMATRIX rotMat = XMMatrixRotationQuaternion(XMLoadFloat4(&toParentData.rotation));
		XMMATRIX toParentMatrix = XMMatrixTranspose(scaleMat * rotMat * transMat);

		XMMATRIX toParent = XMMatrixMultiply(XMMatrixTranspose(XMLoadFloat4x4(&p_Joints[i].m_JointOffsetMatrix)),
			toParentMatrix);
		XMStoreFloat4x4(&m_LocalTransforms[i], toParent);
	}

	updateFinalTransforms();
}

MatrixDecomposed Animation::updateKeyFrameInformation(const Joint& p_Joint, unsigned int p_CurrentTrack,
	const MatrixDecomposed& p_ToParentData)
{
	MatrixDecomposed tempData;
	if(m_Tracks[p_CurrentTrack].clip->m_AnimationSpeed > 0)
	{
		tempData = p_Joint.interpolateEx(m_Tracks[p_CurrentTrack].currentFrame, m_Tracks[p_CurrentTrack].destinationFrame);
	}
	else
	{
		tempData = p_Joint.interpolateEx(m_Tracks[p_CurrentTrack].destinationFrame, m_Tracks[p_CurrentTrack].currentFrame);
	}

	if(m_Tracks[p_CurrentTrack].fadeIn)
	{
		return p_Joint.interpolateEx(p_ToParentData, tempData, (m_Tracks[p_CurrentTrack].fadedFrames /
			(float)m_Tracks[p_CurrentTrack].clip->m_FadeInFrames) * m_Tracks[p_CurrentTrack].clip->m_Weight *
			m_Tracks[p_CurrentTrack].dynamicWeight);
	}
	else if(m_Tracks[p_CurrentTrack].fadeOut && !m_Tracks[p_CurrentTrack].clip->m_Loop)
	{
		return p_Joint.interpolateEx(p_ToParentData, tempData, 1.0f - ((m_Tracks[p_CurrentTrack].fadedFrames /
			(float)m_Tracks[p_CurrentTrack].clip->m_FadeOutFrames) * m_Tracks[p_CurrentTrack].clip->m_Weight) *
			m_Tracks[p_CurrentTrack].dynamicWeight );
	}
	else
	{
		return p_Joint.interpolateEx(p_ToParentData, tempData, m_Tracks[p_CurrentTrack].clip->m_Weight *
			m_Tracks[p_CurrentTrack].dynamicWeight );
	}
}

void Animation::checkFades()
{
	for (int i = 1; i < 6; i++)
	{
		if (!m_Tracks[i].active)
		{
			continue;
		}

		if (m_Tracks[i].fadeIn)
		{
			if ((float)m_Tracks[i].clip->m_FadeInFrames <= m_Tracks[i].fadedFrames)
			{
				m_Tracks[i].fadeIn = false;
				m_Tracks[i].fadedFrames = 0.0f;

				if(i == 1 || i == 3 || i == 5)
				{
					if(!m_Tracks[i].clip->m_Layered && m_Tracks[i].clip->m_Loop)
					{
						m_Tracks[i - 1] = m_Tracks[i];
						m_Tracks[i].active = false;
						m_Tracks[i - 1].active = true;
					}
				}
			}
		}
		if (!m_Tracks[i].fadeIn && m_Tracks[i].fadeOut)
		{
			if ((float)m_Tracks[i].clip->m_FadeOutFrames <= m_Tracks[i].fadedFrames)
			{
				m_Tracks[i].fadeOut = false;
				m_Tracks[i].fadedFrames = 0.0f;
				m_Tracks[i].active = false;
			}
		}
		// If the clip is supposed to fade out and is closing in on the end of it's duration, start playing the next clip in the queue.
		if( (float)(m_Tracks[i].clip->m_End - m_Tracks[i].clip->m_FadeOutFrames - 1) < m_Tracks[i].currentFrame && m_Tracks[i].clip->m_FadeOut)
		{
			playQueuedClip(i);
		}
	}
}

void Animation::updateTimeStamp(float p_DeltaTime)
{
	static const float keyfps = 24.0f;

	for (int i = 0; i < 6; i++)
	{
		if( m_Tracks[i].active )
		{
			float frameStep = p_DeltaTime * keyfps * m_Tracks[i].clip->m_AnimationSpeed;
			m_Tracks[i].currentFrame += frameStep;

			if (m_Tracks[i].fadeIn)
				m_Tracks[i].fadedFrames += abs(frameStep);
			else if ( m_Tracks[i].fadeOut && !m_Tracks[i].clip->m_Loop)
			{
				if( (float)(m_Tracks[i].clip->m_End - m_Tracks[i].clip->m_FadeOutFrames - 1) <= m_Tracks[i].currentFrame)
					m_Tracks[i].fadedFrames += abs(frameStep);
			}

			const unsigned int numKeyframes = (unsigned int)m_Tracks[i].clip->m_End;
	
			if (m_Tracks[i].clip->m_AnimationSpeed > 0.0f)
			{
				m_Tracks[i].destinationFrame = m_Tracks[i].currentFrame + 1.0f;
				if (m_Tracks[i].currentFrame >= (float)(numKeyframes))
				{
					if(m_Tracks[i].clip->m_Loop)
						m_Tracks[i].currentFrame = (float)m_Tracks[i].clip->m_Start;
					else
						m_Tracks[i].active = false;
				}
				if (m_Tracks[i].destinationFrame >= (float)(numKeyframes))
				{
					if(m_Tracks[i].clip->m_Loop)
					{
						float dummy;
						float frac = modff(m_Tracks[i].destinationFrame, &dummy);
						m_Tracks[i].destinationFrame = m_Tracks[i].clip->m_Start + frac;
					}
					else
						m_Tracks[i].active = false;
				}
			}
			else
			{
				if (m_Tracks[i].currentFrame <= m_Tracks[i].clip->m_Start)
				{
					if(m_Tracks[i].clip->m_Loop)
						m_Tracks[i].currentFrame = (float)(numKeyframes);
					else
						m_Tracks[i].active = false;
				}
				m_Tracks[i].destinationFrame = m_Tracks[i].currentFrame - 1.0f;
				if (m_Tracks[i].destinationFrame <= m_Tracks[i].clip->m_Start)
				{
					if(m_Tracks[i].clip->m_Loop)
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

bool Animation::affected(int p_ID, string p_FirstAffectedJoint)
{
	const std::vector<Joint>& p_Joints = m_Data->joints;

	// FirstAffectedJode = FAJ
	if (p_Joints[p_ID].m_JointName == p_FirstAffectedJoint) // The FAJ has been found.
		return true;
	else if(p_ID == 0) // The root joint has been reached and the FAJ has not been found.
		return false;
	else return affected(p_Joints.at(p_ID).m_Parent - 1, p_FirstAffectedJoint); // Neither the root nor the FAJ has been found yet, keep looking.
}

const vector<DirectX::XMFLOAT4X4>& Animation::getFinalTransform() const
{
	return m_FinalTransform;
}

void Animation::applyIK_ReachPoint(const std::string& p_GroupName, const DirectX::XMFLOAT3& p_Position, XMFLOAT4X4 p_WorldMatrix, float p_Weight)
{
	auto it = m_Data->ikGroups.find(p_GroupName);
	if (it == m_Data->ikGroups.end())
	{
		return;
	}

	const IKGroup& p_Group = it->second;

	const std::vector<Joint>& p_Joints = m_Data->joints;

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

	XMMATRIX world = XMLoadFloat4x4(&p_WorldMatrix);

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

	XMVECTOR nmlStartToJoint = XMVector3Normalize(startToJoint);
	XMVECTOR nmlJointToEnd = XMVector3Normalize(jointToEnd);

	float currentJointAngle = XMScalarACos(XMVector3Dot(-nmlStartToJoint, nmlJointToEnd).m128_f32[0]);
	float diffJointAngle = wantedJointAngle - currentJointAngle;

	XMVECTOR rotationAxis = XMVector3Cross(-nmlStartToJoint, nmlJointToEnd);
	rotationAxis = XMVector3Normalize(rotationAxis);
	if (XMVector3LengthSq(rotationAxis).m128_f32[0] < 0.5f)
		return;

	XMVECTOR objectJointToStart = XMVector4Transform(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LocalTransforms[middleJoint->m_ID - 1]))));
	XMVECTOR objectJointToEnd = XMVector4Transform(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMMatrixTranspose(XMLoadFloat4x4(&m_LocalTransforms[endJoint->m_ID - 1])));

	XMVECTOR objectRotationAxis = XMVector3Cross(objectJointToStart, objectJointToEnd);
	objectRotationAxis = XMVector3Normalize(objectRotationAxis);

	XMMATRIX rotation = XMMatrixRotationAxis(-objectRotationAxis, diffJointAngle * p_Weight);

	// Rotate the local transform of the "elbow" joint
	XMStoreFloat4x4(&m_LocalTransforms[middleJoint->m_ID - 1],
		XMMatrixMultiply(XMLoadFloat4x4(&m_LocalTransforms[middleJoint->m_ID - 1]), rotation));

	// Move the end joint in world space
	XMVECTOR worldHingeAxis = rotationAxis;
	worldHingeAxis = XMVectorSetW(worldHingeAxis, 0.f);
	rotation = XMMatrixRotationAxis(worldHingeAxis, diffJointAngle);
	XMVECTOR newJointToEnd = XMVector3Transform(jointToEnd, rotation);
	newJointToEnd = XMVectorSetW(newJointToEnd, 0.f);

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
	
	// Rotate the local transform of the "shoulder" joint
	rotation = XMMatrixRotationAxis(localAxis, -localAngle * p_Weight);
	XMStoreFloat4x4(&m_LocalTransforms[baseJoint->m_ID - 1],
		XMMatrixMultiply(XMLoadFloat4x4(&m_LocalTransforms[baseJoint->m_ID - 1]), rotation));

	// Update the resulting child transformations
	updateFinalTransforms();
}

DirectX::XMFLOAT3 Animation::getJointPos(const string& p_JointName, XMFLOAT4X4 p_WorldMatrix)
{
	const std::vector<Joint>& p_Joints = m_Data->joints;
	
	if(m_FinalTransform.size() > 0)
	{
		for (const auto& joint : p_Joints)
		{
			if (joint.m_JointName == p_JointName)
			{
				// The joints' positions in world space is the zero vector in joint space transformed to world space.
				XMMATRIX jointCombinedTransform = XMMatrixMultiply(
					XMLoadFloat4x4(&p_WorldMatrix),
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

		throw InvalidArgument("Joint does not exist: '" + p_JointName + "'", __LINE__, __FILE__);
	}
	return XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Animation::updateFinalTransforms()
{
	const std::vector<Joint>& p_Joints = m_Data->joints;

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
		//result = offSet;

		result = XMMatrixMultiply(flipMatrix, result);

		XMStoreFloat4x4(&m_FinalTransform[i], result);

		// *** Save for debugging of new animated objects. ***
		//XMMATRIX identity = XMMatrixIdentity();
		//XMStoreFloat4x4(&m_FinalTransform[i], identity);
		//XMStoreFloat4x4(&m_FinalTransform[i], offSet);
	}
}

void Animation::playClip( const std::string& p_ClipName, bool p_Override )
{
	auto clip = m_Data->animationClips.find(p_ClipName);
	if (clip == m_Data->animationClips.end())
	{
		return;
	}

	const AnimationClip* p_Clip = &clip->second;

	int track = p_Clip->m_DestinationTrack;
	if(p_Override)
	{
		m_Tracks[track + 1].clip = p_Clip;
		m_Tracks[track + 1].fadedFrames = 0.0f;
		m_Tracks[track + 1].active = true;
		m_Tracks[track + 1].currentFrame = (float)p_Clip->m_Start;
		m_Tracks[track + 1].dynamicWeight = 1.0f;
		m_Tracks[track + 1].fadeIn = p_Clip->m_FadeIn;
		m_Tracks[track + 1].fadeOut = p_Clip->m_FadeOut;

		for (unsigned int i = track + 2; i < 6; i++)
		{
			m_Tracks[i].active = false;
		}
	}
	else
	{
		if(m_Tracks[track].active)
		{
			if(m_Tracks[track + 1].active)
			{	
				m_Tracks[track].clip			= 	m_Tracks[track + 1].clip;
				m_Tracks[track].fadedFrames		= 	m_Tracks[track + 1].fadedFrames;
				m_Tracks[track].active			= 	m_Tracks[track + 1].active;
				m_Tracks[track].currentFrame	= 	m_Tracks[track + 1].currentFrame;
				m_Tracks[track].dynamicWeight	= 	m_Tracks[track + 1].dynamicWeight;
				m_Tracks[track].fadeIn			= 	m_Tracks[track + 1].fadeIn;
				m_Tracks[track].fadeOut			= 	m_Tracks[track + 1].fadeOut;
			}

			m_Tracks[track + 1].clip = p_Clip;
			m_Tracks[track + 1].fadedFrames = 0.0f;
			m_Tracks[track + 1].active = true;
			m_Tracks[track + 1].currentFrame = (float)p_Clip->m_Start;
			m_Tracks[track + 1].dynamicWeight = 1.0f;
			m_Tracks[track + 1].fadeIn = p_Clip->m_FadeIn;
			m_Tracks[track + 1].fadeOut = p_Clip->m_FadeOut;
		}
		else
		{
			m_Tracks[track].clip = p_Clip;
			m_Tracks[track].fadedFrames = 0.0f;
			m_Tracks[track].active = true;
			m_Tracks[track].currentFrame = (float)p_Clip->m_Start;
			m_Tracks[track].dynamicWeight = 1.0f;
			m_Tracks[track].fadeIn = p_Clip->m_FadeIn;
			m_Tracks[track].fadeOut = p_Clip->m_FadeOut;
		}
	}

	purgeQueue(track);
}

void Animation::queueClip( const std::string& p_Clip )
{
	auto clip = m_Data->animationClips.find(p_Clip);
	if (clip == m_Data->animationClips.end())
	{
		return;
	}

	m_Queue.push_back(&clip->second);
}

bool Animation::playQueuedClip(int p_Track)
{
	if (p_Track == 1 || p_Track == 3 || p_Track == 5)
		p_Track--;

	if(!m_Queue.empty())
	{
		for (unsigned int i = 0; i < m_Queue.size(); i++)
		{
			if (m_Queue[i]->m_DestinationTrack == p_Track)
			{
				playClip(m_Queue[i]->m_ClipName, false);
				return true;
			}
		}
	}
	return false;
}

void Animation::changeWeight(int p_Track, float p_Weight)
{
	if(p_Track > 0 && p_Track < 6)
	{
		m_Tracks[p_Track].dynamicWeight = p_Weight;
		m_Tracks[p_Track + 1].dynamicWeight = p_Weight;
	}
}

void Animation::applyLookAtIK(const std::string& p_GroupName, const DirectX::XMFLOAT3& p_Position, DirectX::XMFLOAT4X4 p_WorldMatrix, float p_MaxAngle)
{
	auto it = m_Data->ikGroups.find(p_GroupName);
	if (it == m_Data->ikGroups.end())
	{
		return;
	}

	const IKGroup& p_Group = it->second;

	const std::vector<Joint>& p_Joints = m_Data->joints;

	XMFLOAT4 targetData(p_Position.x, p_Position.y, p_Position.z, 1.f);
	XMVECTOR target = XMLoadFloat4(&targetData);

	const Joint* headJoint = nullptr;

	for (unsigned int i = 0; i < p_Joints.size(); i++)
	{
		const Joint& joint = p_Joints[i];

		if (joint.m_JointName == p_Group.m_Hand)
		{
			headJoint = &joint;
			break;
		}
	}

	// The algorithm requires all three joints
	if (headJoint == nullptr)
	{
		return;
	}

	XMMATRIX world = XMLoadFloat4x4(&p_WorldMatrix);

	// Calculate matrices for transforming vectors from joint spaces to world space
	XMMATRIX headCombinedTransform = XMMatrixMultiply(
		world,
		XMMatrixMultiply(
			XMLoadFloat4x4(&m_FinalTransform[headJoint->m_ID - 1]),
			XMLoadFloat4x4(&headJoint->m_TotalJointOffset)));
	XMFLOAT4X4 headCombinedTransformedData;
	XMStoreFloat4x4(&headCombinedTransformedData, headCombinedTransform);

	// The joints' positions in world space is the zero vector in joint space transformed to world space.
	XMFLOAT4 headPosition(headCombinedTransformedData._14, headCombinedTransformedData._24,
		headCombinedTransformedData._34, 1.f); 

	XMVECTOR headPositionV = XMLoadFloat4(&headPosition);

	XMVECTOR headToTarget =  target - headPositionV;

	float wantedJointAngle = 0.f;

	// Normalize look at target
	headToTarget = XMVector3Normalize(headToTarget);
	// Get the standard look at vector
	XMVECTOR headForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	headForward = XMVector3Transform(headForward, world);
	headForward = XMVector3Normalize(headForward);
	headForward.m128_f32[0] = -headForward.m128_f32[0];

	// Get rotation axis and angle
	XMVECTOR rotationAxis;
	rotationAxis = XMVector3Cross(headForward, headToTarget);
	rotationAxis = XMVector3Normalize(rotationAxis);
	XMMATRIX headCombinedTransformInverse = XMMatrixInverse(nullptr, headCombinedTransform);
	rotationAxis = XMVector3Transform(rotationAxis, headCombinedTransform);
	wantedJointAngle = acosf(XMVector3Dot(headForward, headToTarget).m128_f32[0]);

	// Limit angle
	wantedJointAngle = std::min( wantedJointAngle, p_MaxAngle );
	// Apply the transformation to the bone
	XMMATRIX rotation;
	rotation = XMMatrixRotationAxis(rotationAxis, wantedJointAngle);

	XMStoreFloat4x4(&m_LocalTransforms[headJoint->m_ID - 1],
	XMMatrixMultiply(XMLoadFloat4x4(&m_LocalTransforms[headJoint->m_ID - 1]), rotation));

	// Update the resulting child transformations
	updateFinalTransforms();
}

void Animation::setAnimationData(AnimationData::ptr p_Data)
{
	m_Data = p_Data;
	playClip("default", true);
}

const AnimationData::ptr Animation::getAnimationData() const
{
	return m_Data;
}

void Animation::purgeQueue(const unsigned int p_Track)
{
	if(!m_Queue.empty())
	{
		auto start = std::remove_if(m_Queue.begin(), m_Queue.end(), [&] (const AnimationClip* a){ return a->m_DestinationTrack == p_Track; });
		m_Queue.erase(start, m_Queue.end());
	}
}