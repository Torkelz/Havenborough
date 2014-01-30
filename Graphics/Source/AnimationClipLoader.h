#pragma once
#include "AnimationClip.h"
#include <map>
#include <string>

class MattiasLucaseXtremeLoader
{
private:

public:
	MattiasLucaseXtremeLoader();
	~MattiasLucaseXtremeLoader();

	std::map<std::string, AnimationClip> loadAnimationClip(std::string);
	std::map<std::string, IKGroup> loadIKGroup(std::string);
};