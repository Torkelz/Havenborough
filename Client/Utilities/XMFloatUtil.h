#pragma once
#include <DirectXMath.h>

struct Vector2
{
	float x;
	float y;

	Vector2() : x(0.0f), y(0.0f)
	{}

	Vector2(float _x, float _y) : x(_x), y(_y)
	{}
};

struct Vector3 : Vector2
{
	float z;

	Vector3() : Vector2(0.0f, 0.0f), z(0.0f)
	{}

	Vector3(float _x, float _y, float _z) : Vector2(_x, _y), z(_z)
	{}
};

struct Vector4 : Vector3
{
	float w;

	Vector4() : Vector3(0.0f, 0.0f, 0.0f), w(0.0f)
	{}

	Vector4(float _x, float _y, float _z, float _w) : Vector3(_x, _y, _z), w(_w)
	{}

	inline Vector4 operator-(Vector4 p_vec)
	{
		this->x -= p_vec.x;
		this->y -= p_vec.y;
		this->z -= p_vec.z;
		this->w -= p_vec.w;

		return *this;
	}
};

inline DirectX::XMFLOAT3 vector3ToXMFLOAT3(Vector3* p_vector)
{
	DirectX::XMFLOAT3 vector;
	vector = DirectX::XMFLOAT3(p_vector->x, p_vector->y, p_vector->z);

	return vector;
};

inline DirectX::XMFLOAT4 vector4ToXMFLOAT4(Vector4* p_vector)
{
	DirectX::XMFLOAT4 vector = DirectX::XMFLOAT4(p_vector->x, p_vector->y, p_vector->z, p_vector->w);

	return vector;
};

inline Vector3 XMFLOAT3ToVector3(DirectX::XMFLOAT3* p_vector)
{
	Vector3 vector = Vector3(p_vector->x, p_vector->y, p_vector->z);

	return vector;
};

inline Vector4 XMFLOAT4ToVector4(DirectX::XMFLOAT4* p_vector)
{
	Vector4 vector = Vector4(p_vector->x, p_vector->y, p_vector->z, p_vector->w);

	return vector;
};

inline DirectX::XMVECTOR Vector4ToXMVECTOR(Vector4* p_vector)
{
	DirectX::XMVECTOR vector = DirectX::XMVectorSet(p_vector->x, p_vector->y, p_vector->z, p_vector->w);

	return vector;
};

inline Vector4 XMVECTORToVector4(DirectX::XMVECTOR* p_vector)
{
	Vector4 vector = Vector4(p_vector->m128_f32[0], p_vector->m128_f32[1], p_vector->m128_f32[2], p_vector->m128_f32[3]);

	return vector;
};

inline DirectX::XMFLOAT2 Vector2ToXMFLOAT2(Vector2 _p)
{
	return DirectX::XMFLOAT2(_p.x, _p.y);
}

inline DirectX::XMFLOAT3 Vector3ToXMFLOAT3(Vector3 _p)
{
	return DirectX::XMFLOAT3(_p.x, _p.y, _p.z);
}

inline DirectX::XMFLOAT4 Vector3ToXMFLOAT4(Vector3 _p, float _w)
{
	return DirectX::XMFLOAT4(_p.x, _p.y, _p.z, _w);
}

inline DirectX::XMFLOAT4 Vector4ToXMFLOAT4(Vector4 _p)
{
	return DirectX::XMFLOAT4(_p.x, _p.y, _p.z, _p.w);
}