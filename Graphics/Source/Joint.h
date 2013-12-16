#pragma once

#include "LoaderStructs.h"

#include <string>
#include <vector>

#include <DirectXMath.h>

class Joint
{
public:
	std::string m_JointName;
	int m_ID;
	int m_Parent;
	DirectX::XMFLOAT4X4 m_JointOffsetMatrix;
	std::vector<KeyFrame> m_JointAnimation;

	void interpolate(float t, DirectX::XMFLOAT4X4& transform);
};
