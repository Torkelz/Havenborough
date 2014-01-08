#pragma once
#include "../../Client/Utilities/Util.h"
typedef unsigned int BodyHandle;

enum class BoundingVolumeType
{
	SPHERE,
	AABB,
	OBB,
	HULL
};

enum class Type
{
	NONE,
	SPHEREVSSPHERE,
	AABBVSSPHERE,
	AABBVSAABB,
	OBBVSOBB,
	OBBVSSPHERE,
	OBBVSAABB,
	HULLVSSPHERE,
	VSEDGE
};

struct Triangle
{
	Vector4 corners[3];

	Triangle(){}
	Triangle(Vector4 p_Corner1, Vector4 p_Corner2, Vector4 p_Corner3)
	{
		corners[0] = p_Corner1;
		corners[1] = p_Corner2;
		corners[2] = p_Corner3;
	}
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
	bool			isEdge;

	HitData()
	{
		colPos		= Vector4(0.f, 0.f, 0.f, 0.f);
		colNorm		= Vector4(0.f, 0.f, 0.f, 0.f);
		intersect	= false;
		colType		= Type::NONE;
		colLength	= -1.f;
		collider = collisionVictim = 0;
		isEdge = false;
	}
};
