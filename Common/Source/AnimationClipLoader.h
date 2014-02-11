#pragma once
#include "AnimationClip.h"
#include <map>
#include <string>

class MattiasLucaseXtremeLoader
{
private:

public:
	//MattiasLucaseXtremeLoader();
	//~MattiasLucaseXtremeLoader();

	static std::map<std::string, AnimationClip> loadAnimationClip(std::string p_Filename);
	static std::map<std::string, IKGroup> loadIKGroup(std::string p_Filename);
	static std::map<std::string, AnimationPath> loadAnimationPath(std::string p_Filename);
};