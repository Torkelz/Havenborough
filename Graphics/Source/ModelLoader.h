#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include <memory>

struct Material
{
	std::string m_Diffuse;
	std::string m_Normal;
	std::string m_Specular;
};

struct Vertex
{
	DirectX::XMFLOAT3 m_Pos;
	DirectX::XMFLOAT3 m_Tangent;
	DirectX::XMFLOAT3 m_Normal;
	DirectX::XMFLOAT2 m_Texture;
};

class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();
	bool loadFile(std::string p_Filename);
private:
	std::unique_ptr<DirectX::XMFLOAT3> m_Pos;
	std::unique_ptr<DirectX::XMFLOAT3> m_Tangents;
	std::unique_ptr<DirectX::XMFLOAT3> m_Normals;
	std::unique_ptr<DirectX::XMFLOAT2> m_Texture;
};