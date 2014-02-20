#include "AnimationClipLoader.h"
#include <fstream>
#include <sstream>

std::map<std::string, AnimationClip> MattiasLucaseXtremeLoader::loadAnimationClip(std::string p_Filename)
{
	const char* filename = p_Filename.c_str();
	std::map<std::string, AnimationClip> returnValue;
	AnimationClip ac = AnimationClip();
	std::ifstream input(filename, std::ifstream::in);

	if(input)
	{
		std::string line, key;
		std::stringstream stringstream;

		while (!input.eof() && std::getline(input, line))
		{
			key = "";
			stringstream = std::stringstream(line);
			stringstream >> key >> std::ws;
			if(strcmp(key.c_str(), "*Clip") == 0)
			{
					std::string line, key;

					// Name
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_ClipName;

					// Speed
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_AnimationSpeed;

					// Start frame
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_Start;
					ac.m_Start--;

					// End frame
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_End;
					ac.m_End--;

					// isLoopable
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_Loop;

					// First joint affected
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_FirstJoint;

					// Destination track
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_DestinationTrack;
					
					// Layer animation
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_Layered;
					
					// Fade in animation
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_FadeIn;
					
					// Fade in frames
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_FadeInFrames;
					
					// Fade out animation
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_FadeOut;
					
					// Fade out frames
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_FadeOutFrames;

					// Off track weight
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_Weight;

					returnValue.insert( std::pair<std::string, AnimationClip>(ac.m_ClipName, ac) );
			}
		}
	}
	else 
	{
		// No file found.
		returnValue.insert(std::pair<std::string, AnimationClip>("default", ac));
	}
	
	return returnValue;
}

std::map<std::string, IKGroup> MattiasLucaseXtremeLoader::loadIKGroup(std::string p_Filename)
{
	const char* filename = p_Filename.c_str();
	std::map<std::string, IKGroup> returnValue;
	IKGroup ik = IKGroup();
	std::ifstream input(filename, std::ifstream::in);

	if(input)
	{
		std::string line, key;
		std::stringstream stringstream;

		while (!input.eof() && std::getline(input, line))
		{
			key = "";
			stringstream = std::stringstream(line);
			stringstream >> key >> std::ws;
			if(strcmp(key.c_str(), "*IKgroup") == 0)
			{
					std::string line, key;

					// Name
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ik.m_GroupName;

					// Shoulder joint
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ik.m_Shoulder;

					// Elbow joint
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ik.m_Elbow;

					// Hand joint
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ik.m_Hand;

					returnValue.insert( std::pair<std::string, IKGroup>(ik.m_GroupName, ik) );
			}
		}
	}
	else 
	{
		// No file found.
		returnValue.insert(std::pair<std::string, IKGroup>("default", ik));
	}
	
	return returnValue;
}

std::map<std::string, AnimationPath> MattiasLucaseXtremeLoader::loadAnimationPath(std::string p_Filename)
{
	const char* filename = p_Filename.c_str();
	std::map<std::string, AnimationPath> returnValue;
	
	std::ifstream input(filename, std::ifstream::in);

	if(input)
	{
		std::string line, key;
		std::stringstream stringstream;

		while (!input.eof() && std::getline(input, line))
		{
			key = "";
			stringstream = std::stringstream(line);
			stringstream >> key >> std::ws;
			if(strcmp(key.c_str(), "*Path") == 0)
			{
				AnimationPath path = AnimationPath();
					std::string line, key;

					// Name
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> path.m_PathName;
					
					std::getline(input, line);
					do
					{
						float value, timestamp;
						bool y = false;

						stringstream = std::stringstream(line);
						stringstream >> key >> value;
						
						y = key == "y" ? true : false;

						std::getline(input, line);
						stringstream = std::stringstream(line);
						stringstream >> key >> timestamp;

						if (y)
							path.m_YPath.push_back(DirectX::XMFLOAT2(value, timestamp));
						else
							path.m_ZPath.push_back(DirectX::XMFLOAT2(value, timestamp));

						std::getline(input, line);
					}while(!input.eof() && line != "*EndPath");

					returnValue.insert( std::pair<std::string, AnimationPath>(path.m_PathName, path) );
			}
		}
	}
	else 
	{
		// No file found.
		returnValue.insert(std::pair<std::string, AnimationPath>("default", AnimationPath()));
	}
	
	return returnValue;
}