#pragma once

#include "AnimationStructs.h"

#include <map>
#include <fstream>
#include <sstream>

class MattiasLucaseXtremeLoader
{
private:

public:
	MattiasLucaseXtremeLoader();
	~MattiasLucaseXtremeLoader();

	std::map<std::string, AnimationClip> loadAnimationClip(std::string);
	std::map<std::string, IKGroup> loadIKGroup(std::string);
};

