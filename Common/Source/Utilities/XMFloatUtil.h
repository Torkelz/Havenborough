#pragma once
#include <DirectXMath.h>
#include <ostream>

struct Vector2 : public DirectX::XMFLOAT2
{
	Vector2()
		: DirectX::XMFLOAT2(0.f, 0.f)
	{}

	Vector2(float _x, float _y)
		: DirectX::XMFLOAT2(_x, _y)
	{}

	bool operator==(const Vector2& p_Right) const
	{
		return x == p_Right.x && y == p_Right.y;
	}

	Vector2 operator*(float p_Right) const
	{
		return Vector2(x * p_Right, y * p_Right);
	}
};

struct Vector3 : public DirectX::XMFLOAT3
{
	Vector3()
		: DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
	{}
	Vector3(const DirectX::XMFLOAT3& _vec)
		: DirectX::XMFLOAT3(_vec)
	{}
	
	Vector3(float _x, float _y, float _z)
		: DirectX::XMFLOAT3(_x, _y, _z)
	{}

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

struct Vector4 : public DirectX::XMFLOAT4
{
	Vector4()
		: DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f)
	{}
	Vector4(const DirectX::XMFLOAT4& _vec)
		: DirectX::XMFLOAT4(_vec)
	{}
	Vector4(const DirectX::XMVECTOR& _vec)
	{
		DirectX::XMStoreFloat4(this, _vec);
	}

	Vector4(float _x, float _y, float _z, float _w)
		: DirectX::XMFLOAT4(_x, _y, _z, _w)
	{}
	
	Vector4(Vector3& _vec, float _w)
		: DirectX::XMFLOAT4(_vec.x, _vec.y, _vec.z, _w)
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

inline Vector3 XMFLOAT4ToVector3(const DirectX::XMFLOAT4 *p_vector)
{
	return Vector3(p_vector->x, p_vector->y, p_vector->z);
};

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

inline DirectX::XMVECTOR Vector4ToXMVECTOR(const Vector4 *p_vector)
{
	return DirectX::XMVectorSet(p_vector->x, p_vector->y, p_vector->z, p_vector->w);
};