#include <DirectXMath.h>
#include <vector>

struct KeyFrame
{
	DirectX::XMFLOAT3 m_Trans;
	DirectX::XMFLOAT4 m_Rot;
	DirectX::XMFLOAT3 m_Scale;
};

struct Joint
{
	std::string m_JointName;
	int m_ID;
	int m_Parent;
	DirectX::XMFLOAT4X4 m_JointOffsetMatrix;
	std::vector<KeyFrame> m_JointAnimation;
};

struct VertexAnimation
{
	DirectX::XMFLOAT4 m_Position;
	DirectX::XMFLOAT3 m_Normal;
	DirectX::XMFLOAT2 m_UV;
	DirectX::XMFLOAT3 m_Tangent;
	DirectX::XMFLOAT3 m_Binormal;
	DirectX::XMFLOAT3 m_Weight;
	DirectX::XMINT4 m_Joint;
};
	
struct Vertex
{
	DirectX::XMFLOAT4 m_Position;
	DirectX::XMFLOAT3 m_Normal;
	DirectX::XMFLOAT2 m_UV;
	DirectX::XMFLOAT3 m_Tangent;
	DirectX::XMFLOAT3 m_Binormal;
};

struct BoundingVolume
{
	DirectX::XMFLOAT4 m_Postition;
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