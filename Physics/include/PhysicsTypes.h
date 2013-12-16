#pragma once

typedef unsigned int BodyHandle;

enum class BoundingVolumeType
{
	SPHERE,
	AABB,
	OBB
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
	VSEDGE
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

struct Matrix4x4
{
	Vector4 M[4];

	Matrix4x4()
	{
		M[0] = Vector4(1.f, 0.f, 0.f, 0.f);
		M[1] = Vector4(0.f, 1.f, 0.f, 0.f);;
		M[2] = Vector4(0.f, 0.f, 1.f, 0.f);;
		M[3] = Vector4(0.f, 0.f, 0.f, 1.f);;
	}

	Matrix4x4(Vector4 p_column1, Vector4 p_column2, Vector4 p_column3, Vector4 p_column4)
	{
		M[0] = p_column1;
		M[1] = p_column2;
		M[2] = p_column3;
		M[3] = p_column4;
	}

	Matrix4x4(Vector4 p_Matrix[4])
	{
		M[0] = p_Matrix[0];
		M[1] = p_Matrix[1];
		M[2] = p_Matrix[2];
		M[3] = p_Matrix[3];
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
