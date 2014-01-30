#pragma once
#include <string>

struct AnimationClip
{
	std::string m_ClipName;
	float		m_AnimationSpeed;
	int			m_Start;
	int			m_End;
	bool		m_Loop;
	std::string	m_FirstJoint;
	int			m_DestinationTrack; // 0 is the main track, 1 is the extra track for animations like waving, 2 is another extra track for whole body animations
	bool		m_Layered;
	bool		m_FadeIn;
	int			m_FadeInFrames;
	bool		m_FadeOut;
	int			m_FadeOutFrames;
	float		m_Weight;

	AnimationClip()
	{
		m_ClipName = "default";
		m_AnimationSpeed = 1.0f;
		m_Start = 1;
		m_End = 10;
		m_Loop = true;
		m_FirstJoint = "Hip";
		m_DestinationTrack = 0;
		m_Layered = false;
		m_FadeIn = false;
		m_FadeInFrames = 0;
		m_FadeOut = false;
		m_FadeOutFrames = 0;
		m_Weight = 0.0f;
	}

	AnimationClip( std::string clipName, float animationSpeed, int start, int end, bool loop, std::string firstJoint,
		int destinationTrack, bool layered, bool fadeIn, int fadeInFrames, bool fadeOut, int fadeOutFrames, float p_Weight)
	{
		m_ClipName = clipName;
		m_AnimationSpeed = animationSpeed;
		m_Start = start;
		m_End = end;
		m_Loop = loop;
		m_FirstJoint = firstJoint;
		m_DestinationTrack = destinationTrack;
		m_Layered = layered;
		m_FadeIn = fadeIn;
		m_FadeInFrames = fadeInFrames;
		m_FadeOut = fadeOut;
		m_FadeOutFrames = fadeOutFrames;
		m_Weight = p_Weight;
	}
};
