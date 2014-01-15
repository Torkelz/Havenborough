#pragma once

#include <vector>

struct AnimationClip
{
	float		m_Start;
	float		m_End;
	std::string m_ClipName;
	bool		m_Loop;
	float		m_AnimationSpeed;
	int			m_FirstJoint;
	int			m_PriorityLevel; // 0 is low.

	AnimationClip()
	{
		m_Start = 1.0f;
		m_End = 10.0f;
		m_Loop = true;
		m_AnimationSpeed = 1.0f;
		m_FirstJoint = 0;
		m_PriorityLevel = 0;
		m_ClipName = "default";
	}

	AnimationClip( float start, float end, std::string clipName, bool loop, float animationSpeed, int firstJoint, int priorityLevel)
	{
		m_Start = start;
		m_End = end;
		m_ClipName = clipName;
		m_Loop = loop;
		m_AnimationSpeed = animationSpeed;
		m_FirstJoint = firstJoint;
		m_PriorityLevel = priorityLevel;
	}
};

