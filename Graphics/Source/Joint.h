#pragma once

#include "LoaderStructs.h"

#include <string>
#include <vector>

#include <DirectXMath.h>

/**
 * Joint represents a joint in a skeletal animated mesh,
 * along with the animation for that joint.
 */
class Joint
{
public:
	/**
	 * The name of the joint.
	 */
	std::string m_JointName;
	/**
	 * The internal ID of the joint.
	 */
	int m_ID;
	/**
	 * The internal ID of the joint's parent joint.
	 * A joint ID of 0 represents no parent joint.
	 */
	int m_Parent;
	/**
	 * The matrix offset of the joint that would move a vector from the local
	 * space of the joint's parent to the local space of the actual joint.
	 */
	DirectX::XMFLOAT4X4 m_JointOffsetMatrix;
	/**
	 * A list of all of the animations' keyframes, that is interpolated in between frames.
	 */
	std::vector<KeyFrame> m_JointAnimation;

	/**
	 * Calculate the animation transformation at a certain frame.
	 *
	 * @param p_FrameTime the time point to calculate a transformation for.
	 *			Must be in the range [0.f, n - 1), where n is the number of frames.
	 * @return the interpolated transformation.
	 */
	DirectX::XMFLOAT4X4 interpolate(float p_FrameTime) const;
};
