#pragma once

#include "AnimationStructs.h"

#include <map>
#include <fstream>
#include <sstream>

class AnimationClipLoader
{
private:

public:
	AnimationClipLoader();
	~AnimationClipLoader();

	std::map<std::string, AnimationClip> load(std::string);
};

