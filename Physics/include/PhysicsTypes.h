#pragma once

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
	AABBVSAABB
};

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3()
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}

	Vector3(float p_X, float p_Y, float p_Z)
	{
		x = p_X;
		y = p_Y;
		z = p_Z;
	}
};

struct Vector4
{
	float x;
	float y;
	float z;
	float w;

	Vector4()
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
		w = 0.f;
	}

	Vector4(float p_X, float p_Y, float p_Z, float p_W)
	{
		x = p_X;
		y = p_Y;
		z = p_Z;
		w = p_W;
	}

	inline Vector4 operator-(Vector4 p_vec)
	{
		this->x -= p_vec.x;
		this->y -= p_vec.y;
		this->z -= p_vec.z;
		this->w -= p_vec.w;

		return *this;
	}
};

struct HitData
{
	Vector4			colPos;
	Vector4			colNorm;
	bool			intersect;
	Type			colType;
	float			colLength;
	//BodyHandle		colBody1;

	HitData()
	{
		colPos		= Vector4(0.f, 0.f, 0.f, 0.f);
		colNorm		= Vector4(0.f, 0.f, 0.f, 0.f);
		intersect	= false;
		colType		= Type::NONE;
		colLength	= -1.f;
	}
};
