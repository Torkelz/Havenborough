#pragma once
#include "../../Client/Utilities/Util.h"
typedef unsigned int BodyHandle;

enum class BoundingVolumeType
{
	SPHERE,
	AABB
};

enum class Type
{
	NONE,
	SPHEREVSSPHERE,
	AABBVSSPHERE,
	AABBVSAABB,
	VSEDGE,
};

struct HitData
{
	Vector4			colPos;
	Vector4			colNorm;
	bool			intersect;
	Type			colType;
	float			colLength;
	BodyHandle		collider;
	BodyHandle		collisionVictim;

	HitData()
	{
		colPos		= Vector4(0.f, 0.f, 0.f, 0.f);
		colNorm		= Vector4(0.f, 0.f, 0.f, 0.f);
		intersect	= false;
		colType		= Type::NONE;
		colLength	= -1.f;
		collider = collisionVictim = 0;
	}
};
