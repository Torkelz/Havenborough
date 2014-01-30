#pragma once
#include "AnimationClip.h"
#include <map>
#include <string>

class AnimationClipLoader
{
private:

public:
	AnimationClipLoader();
	~AnimationClipLoader();

	std::map<std::string, AnimationClip> load(std::string);
};