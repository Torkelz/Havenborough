#pragma once
#include "Utilities/XMFloatUtil.h"

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
	OBBVSHULL,
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
	/**
	 * Relative translation of the triangle.
	 * @param p_Position how much to move from last frame.
	 */
	void translate(Vector4 p_Position)
	{
		p_Position.w = 0.f;
		corners[0] = corners[0] + p_Position;
		corners[1] = corners[1] + p_Position;
		corners[2] = corners[2] + p_Position;
	}
	/**
	 * Uniform(same amount in all axis) scaling of the triangle. Used for debug drawing with spheres.
	 * @param p_Size the new size of the triangle, increases the distance between the corners.
	 */
	void uniformScale(float p_Size)
	{
		corners[0] = corners[0] * p_Size;
		corners[1] = corners[1] * p_Size;
		corners[2] = corners[2] * p_Size;
	}
	~Triangle(){};
};

struct HitData
{
	Vector4			colPos;	// cm
	Vector4			colNorm;
	bool			intersect;
	Type			colType;
	float			colLength;	// cm
	BodyHandle		collider;
	BodyHandle		collisionVictim;
	bool			isEdge;
	int			    IDInBody;

	HitData() : colPos(Vector4(0.f, 0.f, 0.f, 0.f)),
		colNorm(Vector4(0.f, 0.f, 0.f, 0.f)),
		intersect(false),
		colType(Type::NONE),
		colLength(-1.0f),
		collider(0),
		collisionVictim(0),
		isEdge(false),
		IDInBody(0)
	{
		//colPos		= Vector4(0.f, 0.f, 0.f, 0.f);
		//colNorm		= Vector4(0.f, 0.f, 0.f, 0.f);
		//intersect	= false;
		//colType		= Type::NONE;
		//colLength	= -1.f;
		//collider = collisionVictim = 0;
		//isEdge = false;
		//IDInBody = 0;
	}
};
