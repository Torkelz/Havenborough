#pragma once
#include <DirectXMath.h>
#include <string>

struct KeyFrame
{
	DirectX::XMFLOAT3 m_Trans;
	DirectX::XMFLOAT4 m_Rot;
	DirectX::XMFLOAT3 m_Scale;
};

struct StaticVertex
{
	DirectX::XMFLOAT4 m_Position;
	DirectX::XMFLOAT3 m_Normal;
	DirectX::XMFLOAT2 m_UV;
	DirectX::XMFLOAT3 m_Tangent;
	DirectX::XMFLOAT3 m_Binormal;
};

struct AnimatedVertex 
{
	DirectX::XMFLOAT4 m_Position;
	DirectX::XMFLOAT3 m_Normal;
	DirectX::XMFLOAT2 m_UV;
	DirectX::XMFLOAT3 m_Tangent;
	DirectX::XMFLOAT3 m_Binormal;
	DirectX::XMFLOAT3 m_Weights;
	DirectX::XMINT4 m_BoneId;
};

struct MaterialBuffer
{
	std::string material;
	int start;
	int length;
};

struct IndexDesc
{
	std::string m_MaterialID;
	int m_Vertex;
	int m_Tangent;
	int m_Normal;
	int m_TextureCoord;
};
	
struct Material
{
	std::string m_MaterialID;
	std::string m_DiffuseMap;
	std::string m_NormalMap;
	std::string m_SpecularMap;
};

struct particlecBuffer
{
	DirectX::XMFLOAT4X4 viewM;
	DirectX::XMFLOAT4X4 projM;
	DirectX::XMFLOAT4	cameraPos;
	DirectX::XMFLOAT2	size;
	//DirectX::XMFLOAT4	centerPos;
	//DirectX::XMFLOAT4	color;
	//DirectX::XMFLOAT4	particleColorDeviation;
	//DirectX::XMFLOAT3	particlePositionDeviation;
	//DirectX::XMFLOAT3	velocityDeviation;
	//DirectX::XMFLOAT2	uvCoord;
};

struct ShaderParticle
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};
