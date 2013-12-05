#pragma once

#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>

class ModelLoader
{
public:
	struct Face
	{
		std::string m_MaterialID;
		std::vector<int> m_Vertex;
		std::vector<int> m_Tangents;
		std::vector<int> m_Normals;
		std::vector<int> m_TextureCoord;
	};
	
	struct Material
	{
		std::string m_MaterialID;
		std::string m_DiffuseMap;
		std::string m_NormalMap;
		std::string m_SpecularMap;
	};

private:
	int m_NumberOfMaterials;
	std::string m_MeshName;
	int m_NumberOfVertices;
	int m_NumberOfTriangles;
	std::vector<DirectX::XMFLOAT3> m_Vertex;
	std::vector<DirectX::XMFLOAT3> m_Tangents;
	std::vector<DirectX::XMFLOAT3> m_Normals;
	std::vector<DirectX::XMFLOAT2> m_TextureCoord;
	std::vector<Face> m_Index;
	std::vector<Material> m_Material;
	std::vector<std::vector<Face>> m_IndexPerMaterial;
public:
	ModelLoader();
	~ModelLoader();
	void clear();
	bool loadFile(std::string p_Filename);
	std::vector<DirectX::XMFLOAT3>  *getVertices();
	std::vector<std::vector<Face>>  *getIndices();
	std::vector<Material>			*getMaterial();
	std::vector<DirectX::XMFLOAT3>  *getTangent();
	std::vector<DirectX::XMFLOAT3>  *getNormal();
	std::vector<DirectX::XMFLOAT2>  *getUV();
};