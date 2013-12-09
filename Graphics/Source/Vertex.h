#ifndef VERTEX_H
#define VERTEX_H
#include <DirectXMath.h>

class Vertex
{
public:
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
	Vertex(){}
	Vertex(DirectX::XMFLOAT3 _position,
		DirectX::XMFLOAT3 _normal,
		DirectX::XMFLOAT2 _uv,
		DirectX::XMFLOAT3 _tangent)
	{
		position = DirectX::XMFLOAT4(_position.x,_position.y,_position.z,1.0f);
		normal = _normal;
		uv = _uv;
		tangent = _tangent;

		//might be wrong
		DirectX::XMStoreFloat3(&binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&tangent),DirectX::XMLoadFloat3(&normal)));
	}

	int getSize()
	{
		return sizeof(DirectX::XMFLOAT4) + ( 3 * sizeof(DirectX::XMFLOAT3) ) + sizeof(DirectX::XMFLOAT2);
	}
};

class AnimatedVertex : public Vertex
{
public:
	DirectX::XMFLOAT3 weights;
	unsigned int boneId[4];
	AnimatedVertex() : Vertex(){}
	AnimatedVertex(
		DirectX::XMFLOAT3 _position,
		DirectX::XMFLOAT3 _normal,
		DirectX::XMFLOAT2 _uv,
		DirectX::XMFLOAT3 _tangent,
		DirectX::XMFLOAT3 _weights,
		DirectX::XMFLOAT4 _boneId) : Vertex( _position, _normal, _uv, _tangent)
	{
		position = DirectX::XMFLOAT4(_position.x,_position.y,_position.z,1.0f);
		normal = _normal;
		uv = _uv;
		tangent = _tangent;
		//might be wrong
		DirectX::XMStoreFloat3(&binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&tangent),DirectX::XMLoadFloat3(&normal)));

		weights = _weights;
		boneId[0] = unsigned int(_boneId.x);
		boneId[1] = unsigned int(_boneId.y);
		boneId[2] = unsigned int(_boneId.z);
		boneId[3] = unsigned int(_boneId.w);
	}

	int getSize()
	{
		return sizeof(DirectX::XMFLOAT4) + ( 4 * sizeof(DirectX::XMFLOAT3) ) + sizeof(DirectX::XMFLOAT2) + (4 * sizeof(unsigned int));
	}
};

#endif