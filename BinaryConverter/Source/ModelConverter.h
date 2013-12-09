#pragma once

#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "ModelLoader.h"

class ModelConverter
{
public:
	struct VertexBuffer
	{
		DirectX::XMFLOAT4 m_Position;
		DirectX::XMFLOAT3 m_Tangent;
		DirectX::XMFLOAT3 m_Normal;
		DirectX::XMFLOAT2 m_UV;
		DirectX::XMFLOAT3 m_Binormal;
		DirectX::XMFLOAT3 m_Weight;
		DirectX::XMFLOAT4 m_Joint;
	};
	
	struct MaterialBuffer
	{
		std::string material;
		int start;
		int length;
	};

private:
	std::string m_MeshName;
	std::vector<ModelLoader::IndexDesc> m_Indices;

	float m_Start, m_End;
	int m_NumberOfFrames;
	const std::vector<DirectX::XMFLOAT3> *m_Vertices, *m_Normals, *m_Tangents;
	const std::vector<DirectX::XMFLOAT2> *m_TextureCoord;
	const std::vector<ModelLoader::Material>* m_Material;
	const std::vector<std::vector<ModelLoader::IndexDesc>> *m_IndexPerMaterial;
	const std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT4>>* m_WeightsList;
	const std::vector<ModelLoader::Joint>* m_ListOfJoints;

	int m_VertexCount;
public:
	
	/**
	 * Constructor.
	 */
	ModelConverter();
	
	/**
	 * Destructor.
	 */
	~ModelConverter();
	
	/**
	 * Use this function to release the memory in loader vectors.
	 */
	void clear();

	void writeFile(std::string p_FilePath);
	
	void setVertices(const std::vector<DirectX::XMFLOAT3>* p_Vertices);

	void setIndices(const std::vector<std::vector<ModelLoader::IndexDesc>>* p_Indices);

	void setMaterial(const std::vector<ModelLoader::Material>* p_Material);

	void setNormals(const std::vector<DirectX::XMFLOAT3>* p_Normals);

	void setTangents(const std::vector<DirectX::XMFLOAT3>* p_Tangents);

	void setTextureCoords(const std::vector<DirectX::XMFLOAT2>* p_TextureCoord);

	void setWeightsList(const std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT4>>* p_WeightsList);

	void setListOfJoints(const std::vector<ModelLoader::Joint>* p_ListOfJoints);

	void setAnimationStartValue(float p_StartTime);

	void setAnimationEndValue(float p_EndTime);

	void setNumberOfFrames(int p_NumberOfFrames);

	void setMeshName(std::string p_MeshName);

private:
	void intToByte(int p_Int, std::ostream* p_Output);
	void floatToByte(float p_Float, std::ostream* p_Output);
	void stringToByte(std::string p_String, std::ostream* p_Output);

	void createHeader(std::ostream* p_Output);
	void createMaterial(std::ostream* p_Output);
	void createMaterialBuffer(std::ostream* p_Output);
	void createVertexBuffer(std::ostream* p_Output);
	void createJointBuffer(std::ostream* p_Output);

	void clearData();
};