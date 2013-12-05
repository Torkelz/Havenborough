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
		int m_Vertex;
		int m_Tangents;
		int m_Normals;
		int m_TextureCoord;
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
	std::vector<std::vector<Face>> getIndices();

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