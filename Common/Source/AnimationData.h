#pragma once

#include "Joint.h"

#include <memory>

struct AnimationData
{
	typedef std::shared_ptr<AnimationData> ptr;

	std::vector<Joint> joints;
};
