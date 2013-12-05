#pragma once

#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>

class ModelLoader
{
public:
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

	struct KeyFrame
	{
		DirectX::XMFLOAT3 m_Trans;
		DirectX::XMFLOAT3 m_Rot;
		DirectX::XMFLOAT3 m_Scale;
	};

private:
	int m_NumberOfMaterials;
	std::string m_MeshName;
	int m_NumberOfVertices;
	int m_NumberOfTriangles;
	float m_Start;
	int m_End,m_NumFrames;
	std::vector<DirectX::XMFLOAT3> m_Vertices;
	std::vector<DirectX::XMFLOAT3> m_Tangents;
	std::vector<DirectX::XMFLOAT3> m_Normals;
	std::vector<DirectX::XMFLOAT2> m_TextureCoord;
	std::vector<IndexDesc> m_Indices;
	std::vector<Material> m_Material;
	std::vector<std::vector<IndexDesc>> m_IndexPerMaterial;
	std::vector<std::pair<DirectX::XMFLOAT4, DirectX::XMFLOAT4>> m_WeightsList;
	std::vector<std::pair<int, int>> m_JointHierarchy;
	std::vector<DirectX::XMFLOAT4X4> m_JointOffsetMatrix;
	std::vector<std::vector<KeyFrame>> m_JointAnimation;
public:
	
	/**
	 * Constructor.
	 */
	ModelLoader();
	
	/**
	 * Destructor.
	 */
	~ModelLoader();
	
	/**
	 * Use this function to release the memory in loader vectors.
	 */
	void clear();
	
	/**
	 * Creates vectors with information from the requested file.
	 *
	 * @param p_FilePath, the absolute path to the requested file.
	 */
	bool loadFile(std::string p_FilePath);

	/**
	 * Returns the stored information about vertices as a vector with Float3 values. 
	 *
	 * @returns a vector of vertices.
	 */
	std::vector<DirectX::XMFLOAT3> getVertices();

	/**
	 * Returns the stored information about indices as a vector with Float3 values.
	 *
	 * @returns a vector of indices.
	 */
	std::vector<std::vector<IndexDesc>> getIndices();

	/**
	 * Returns the stored information about the materials that are used by the model.
	 *
	 * @returns a vector of material structs.
	 */
	std::vector<Material> getMaterial();

	/**
	 * Returns the stored information about normal as a vector with Float3.
	 *
	 * @returns a vector of normals.
	 */
	std::vector<DirectX::XMFLOAT3> getNormals();

	/**
	 * Returns the stored information about tangents as a vector with Float3. 
	 *
	 * @returns a vector of tangents.
	 */
	std::vector<DirectX::XMFLOAT3> getTangents();

	/**
	 * Returns the stored information about uv coordinates as a vector with Float2. 
	 *
	 * @returns a vector of texture coords.
	 */
	std::vector<DirectX::XMFLOAT2> getTextureCoords();
};