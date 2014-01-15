#include "AnimationClipLoader.h"

AnimationClipLoader::AnimationClipLoader(){}
AnimationClipLoader::~AnimationClipLoader(){}

std::map<std::string, AnimationClip> AnimationClipLoader::load(std::string p_Filename)
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

					// Priority level
					std::getline(input, line);
					stringstream = std::stringstream(line);
					stringstream >> key >> ac.m_PriorityLevel;

					returnValue.insert( std::pair<std::string, AnimationClip>(ac.m_ClipName, ac) );
			}
		}
	}
	else 
	{
		returnValue.insert(std::pair<std::string, AnimationClip>("default", ac));
		//Fr�ga Sebastian hur man skriver till loggfilen.
	}
	
	return returnValue;
}