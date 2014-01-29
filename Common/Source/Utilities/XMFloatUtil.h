#pragma once
#include <DirectXMath.h>
#include <ostream>

struct Vector2
{
	float x;
	float y;

	Vector2() : x(0.0f), y(0.0f)
	{}

	Vector2(float _x, float _y) : x(_x), y(_y)
	{}
};

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3() :x(0.0f), y(0.0f), z(0.0f)
	{}
	Vector3(const DirectX::XMFLOAT3& _vec) : x(_vec.x), y(_vec.y), z(_vec.z)
	{}
	
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{}

	operator DirectX::XMFLOAT3() const
	{
		return DirectX::XMFLOAT3(x, y, z);
	}

	Vector3 operator+(const Vector3& p_Right) const
	{
		return Vector3(x + p_Right.x, y + p_Right.y, z + p_Right.z);
	}

	Vector3 operator-(const Vector3& p_Right) const
	{
		return Vector3(x - p_Right.x, y - p_Right.y, z - p_Right.z);
	}

	Vector3 operator*(float p_Right) const
	{
		return Vector3(x * p_Right, y * p_Right, z * p_Right);
	}

	friend std::ostream& operator<<(std::ostream& p_Stream, const Vector3& p_Vec)
	{
		return p_Stream << "(" << p_Vec.x << ", " << p_Vec.y << ", " << p_Vec.z << ")";
	}

	bool operator==(const Vector3& p_Right) const
	{
		return x == p_Right.x && y == p_Right.y && z == p_Right.z;
	}
};

struct Vector4
{
	float x;
	float y;
	float z;
	float w;

	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{}

	Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
	{}

	inline Vector4 operator-(Vector4 p_Right)
	{
		this->x -= p_Right.x;
		this->y -= p_Right.y;
		this->z -= p_Right.z;
		this->w -= p_Right.w;

		return *this;
	}

	Vector4 operator+(const Vector4& p_Right) const
	{
		return Vector4(x + p_Right.x, y + p_Right.y, z + p_Right.z, w + p_Right.w);
	}
	Vector4 operator*(float p_Right) const
	{
		return Vector4(x * p_Right, y * p_Right, z * p_Right, w * p_Right);
	}
	Vector3 xyz()
	{
		return Vector3(x, y, z);
	}
};

inline Vector2 XMFLOAT2ToVector2(const DirectX::XMFLOAT2 *p_vector)
{
	return Vector2(p_vector->x, p_vector->y);
};

inline Vector3 XMFLOAT3ToVector3(const DirectX::XMFLOAT3 *p_vector)
{
	return Vector3(p_vector->x, p_vector->y, p_vector->z);
};

inline Vector3 XMFLOAT4ToVector3(const DirectX::XMFLOAT4 *p_vector)
{
	return Vector3(p_vector->x, p_vector->y, p_vector->z);
};

inline Vector4 XMFLOAT4ToVector4(const DirectX::XMFLOAT4 *p_vector)
{
	return Vector4(p_vector->x, p_vector->y, p_vector->z, p_vector->w);
};

inline Vector4 XMVECTORToVector4(const DirectX::XMVECTOR *p_vector)
{
	return Vector4(p_vector->m128_f32[0], p_vector->m128_f32[1], p_vector->m128_f32[2], p_vector->m128_f32[3]);
};

inline DirectX::XMFLOAT2 Vector2ToXMFLOAT2(const Vector2 *_p)
{
	return DirectX::XMFLOAT2(_p->x, _p->y);
}

inline DirectX::XMFLOAT3 Vector3ToXMFLOAT3(const Vector3 *_p)
{
	return DirectX::XMFLOAT3(_p->x, _p->y, _p->z);
}

inline DirectX::XMFLOAT4 Vector3ToXMFLOAT4(const Vector3 *_p, float _w)
{
	return DirectX::XMFLOAT4(_p->x, _p->y, _p->z, _w);
}

inline DirectX::XMVECTOR Vector3ToXMVECTOR(const Vector3 *_p, float _w)
{
	return DirectX::XMVectorSet(_p->x, _p->y, _p->z, _w);
}

inline DirectX::XMFLOAT3 Vector4ToXMFLOAT3(const Vector4 *_p)
{
	return DirectX::XMFLOAT3(_p->x, _p->y, _p->z);
}

inline DirectX::XMFLOAT4 Vector4ToXMFLOAT4(const Vector4 *_p)
{
	return DirectX::XMFLOAT4(_p->x, _p->y, _p->z, _p->w);
}

inline DirectX::XMVECTOR Vector4ToXMVECTOR(const Vector4 *p_vector)
{
	return DirectX::XMVectorSet(p_vector->x, p_vector->y, p_vector->z, p_vector->w);
};