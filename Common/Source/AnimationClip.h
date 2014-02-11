#pragma once
#include <vector>
#include <DirectXMath.h>

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

struct IKGroup
{
	std::string			m_GroupName;
	std::string			m_Shoulder;
	std::string			m_Elbow;
	std::string			m_Hand;
	DirectX::XMFLOAT3	m_ElbowHingeAxis;

	IKGroup()
	{
		m_GroupName			= "default";
		m_Shoulder			= "defaultShoulder";
		m_Elbow				= "defaultElbow";
		m_Hand				= "defaultHand";
		m_ElbowHingeAxis	= DirectX::XMFLOAT3(0.f, -1.f, 0.f);
	}

	IKGroup(std::string p_GroupName, std::string p_Shoulder, std::string p_Elbow, 
		std::string p_Hand, DirectX::XMFLOAT3 p_ElbowAxis)	
	{
		m_GroupName			= p_GroupName;
		m_Shoulder			= p_Shoulder;
		m_Elbow				= p_Elbow;
		m_Hand				= p_Hand;
		m_ElbowHingeAxis	= p_ElbowAxis;
	}
};

struct AnimationPath
{
	std::string						m_PathName;
	std::vector<DirectX::XMUINT2>	m_YPath;
	std::vector<DirectX::XMUINT2>	m_ZPath;

	AnimationPath()
	{
		m_PathName = "default";
	}

	AnimationPath(std::string p_PathName, std::vector<DirectX::XMUINT2> p_YPath, std::vector<DirectX::XMUINT2> p_ZPath)
	{
		m_PathName	= p_PathName;
		m_YPath		= p_YPath;
		m_ZPath		= p_ZPath;
	}
};