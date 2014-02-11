#pragma once

#include "AnimationClip.h"
#include "Joint.h"

#include <map>
#include <memory>

struct AnimationData
{
	typedef std::shared_ptr<AnimationData> ptr;

	std::vector<Joint> joints;

	/**
	 * The animation clips. Address them via a name. E.g. "Walk", "Run", "Laugh"...
	 */
	std::map<std::string, AnimationClip> animationClips;

	/**
	 * An IK group contains the static data needed to perform IK on three joints. Makes Mattias' life simpler.
	 */
	std::map<std::string, IKGroup> ikGroups;

	/**
	 * Some animations need to force the character to move in a certain path outside of the character's normal
	 * movement. Makes Lucas happy. André brain farted super bad when the loader was written. It smelled.
	 */
	std::map<std::string, AnimationPath> animationPath;
};
