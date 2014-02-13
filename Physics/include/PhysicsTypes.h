#pragma once
#include <Utilities/XMFloatUtil.h>

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
	/**
	* Given point p and the triangle corners in world coordinates, return point in triangle, closest to p
	* @param p_point the point you want to search from
	* @param p_A, corner[0] of triangle in world coordinates
	* @param p_B, corner[1] of triangle in world coordinates
	* @param p_C, corner[2] of triangle in world coordinates
	* @return closest point in the triangle
	*/
	DirectX::XMFLOAT4 findClosestPointOnTriangle(DirectX::XMFLOAT4 const &p_Point, Vector4 const &p_A, Vector4 const &p_B, Vector4 const &p_C)
	{
		DirectX::XMVECTOR a = Vector4ToXMVECTOR(&p_A);
		DirectX::XMVECTOR b = Vector4ToXMVECTOR(&p_B);
		DirectX::XMVECTOR c = Vector4ToXMVECTOR(&p_C);
		DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&p_Point);

		using DirectX::operator-;
		using DirectX::operator*;
		using DirectX::operator+;

		DirectX::XMVECTOR ab = b - a;
		DirectX::XMVECTOR ac = c - a;
		DirectX::XMVECTOR ap = pos - a;

		float d1 = DirectX::XMVector4Dot(ab, ap).m128_f32[0];
		float d2 = DirectX::XMVector4Dot(ac, ap).m128_f32[0];
			
		DirectX::XMFLOAT4 ret;
		if(d1 <= 0.f && d2 <= 0.f)
		{
			DirectX::XMStoreFloat4(&ret, a);
			return ret;
		}

		DirectX::XMVECTOR bp = pos - b;
		float d3 = DirectX::XMVector4Dot(ab, bp).m128_f32[0];
		float d4 = DirectX::XMVector4Dot(ac, bp).m128_f32[0];

		if(d3 >= 0.f && d4 <= d3)
		{
			DirectX::XMStoreFloat4(&ret, b);
			return ret;
		}

		float vc = d1*d4 - d3*d2;
		using DirectX::operator*;
		using DirectX::operator+;

		if(vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
		{
			float v = d1 / (d1 - d3);	
			DirectX::XMStoreFloat4(&ret, a + v * ab);
			return ret;
		}

		DirectX::XMVECTOR cp = pos - c;
		float d5 = DirectX::XMVector3Dot(ab, cp).m128_f32[0];
		float d6 = DirectX::XMVector3Dot(ac, cp).m128_f32[0];

		if(d6 >= 0.f && d5 <= d6)
		{
			DirectX::XMStoreFloat4(&ret, c);
			return ret;
		}

		float vb = d5*d2 - d1*d6;
		if(vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
		{
			float w = d2 / (d2 - d6);
			DirectX::XMStoreFloat4(&ret, a + w * ac);
			return ret;
		}

		float va = d3*d6 - d5*d4;
		if(va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
		{
			float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
			DirectX::XMStoreFloat4(&ret, b + w * ( c - b ));
			return ret;
		}

		float denom = 1.f / ( va + vb + vc);

		float v = vb * denom;
		float w = vc * denom;
		DirectX::XMStoreFloat4(&ret, a + ab * v + ac * w);
		return ret;
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
