#pragma once
#include <vector>
#include <map>
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

	IKGroup()
	{
		m_GroupName			= "default";
		m_Shoulder			= "defaultShoulder";
		m_Elbow				= "defaultElbow";
		m_Hand				= "defaultHand";
	}

	IKGroup(std::string p_GroupName, std::string p_Shoulder, std::string p_Elbow, 
		std::string p_Hand)	
	{
		m_GroupName			= p_GroupName;
		m_Shoulder			= p_Shoulder;
		m_Elbow				= p_Elbow;
		m_Hand				= p_Hand;
	}
};

struct AnimationPath
{
	std::string						m_PathName;
	float							m_Speed;
	std::vector<DirectX::XMFLOAT2>	m_YPath;
	std::vector<DirectX::XMFLOAT2>	m_ZPath;

	AnimationPath()
	{
		m_PathName = "default";
	}

	AnimationPath(std::string p_PathName, float p_Speed, std::vector<DirectX::XMFLOAT2> p_YPath, std::vector<DirectX::XMFLOAT2> p_ZPath)
	{
		m_PathName	= p_PathName;
		m_Speed		= p_Speed;
		m_YPath		= p_YPath;
		m_ZPath		= p_ZPath;
	}
};

struct IKGrab
{
	std::string			m_Target;
	float				m_Position;
	bool				m_Active;
	float				m_Start;
	float				m_End;
	float				m_FadeInTime;
	float				m_FadeOutTime;
	float				m_Faded;
	bool				m_FadeIn;

	IKGrab()
	{
		m_Target = "default";
		m_Position = 0.0f;
	}
};

struct IKGrabShell
{
	std::string							m_Name;
	float								m_Speed;
	std::map<std::string, IKGrab>		m_Grabs;
	float								m_CurrentFrame;
	float								m_Weight;

	void updateTimeStamps(float dt)
	{
		m_CurrentFrame += dt * 24.0f * m_Speed;
		for (auto &grab : m_Grabs)
		{
			if (grab.second.m_Start > m_CurrentFrame)
			{
				grab.second.m_Active = false;
			}
			else if (grab.second.m_End > m_CurrentFrame)
			{
				grab.second.m_Active = true;
			}
			else
			{
				grab.second.m_Active = false;
			}

			if(grab.second.m_FadeIn)
			{
				grab.second.m_Faded += dt * 24.0f * m_Speed;
				if (grab.second.m_Faded > grab.second.m_FadeInTime)
				{
					grab.second.m_FadeIn = false;
					m_Weight = 1.0f;
					grab.second.m_Faded = 0.0f;
				}
				else
				{
					m_Weight = grab.second.m_Faded / grab.second.m_FadeInTime;
				}
			}
			else if (m_CurrentFrame > grab.second.m_End - grab.second.m_FadeOutTime)
			{
				grab.second.m_Faded += dt * 24.0f * m_Speed;
				m_Weight = 1.0f - (grab.second.m_Faded / grab.second.m_FadeOutTime);
			}
		}
	}

	IKGrabShell()
	{
		m_Name = "default";
	}
};