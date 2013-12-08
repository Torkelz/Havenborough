#pragma once

#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>

class ModelConverter
{
public:
	struct Header
	{
		std::string m_modelName;
		int m_numMaterial;
		int m_numVertex;
		int m_numMaterialBuffer;
	};

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

	struct VertexBuffer
	{
		DirectX::XMFLOAT4 m_Position;
		DirectX::XMFLOAT3 m_Tangent;
		DirectX::XMFLOAT3 m_Normal;
		DirectX::XMFLOAT2 m_UV;
		DirectX::XMFLOAT3 m_Binormal;
	};

	struct IndexDesc
	{
		std::string m_MaterialID;
		int m_Vertex;
		int m_Tangent;
		int m_Normal;
		int m_TextureCoord;
	};
	
	struct MaterialBuffer
	{
		std::string material;
		int start;
		int length;
	};

	struct Material
	{
		std::string m_MaterialID;
		std::string m_DiffuseMap;
		std::string m_NormalMap;
		std::string m_SpecularMap;
	};

private:
	
	std::string m_MeshName;
	int m_NumberOfTriangles;
	std::vector<IndexDesc> m_Indices;
	unsigned int m_VertexCount;

	float m_Start, m_End;
	int m_NumberOfFrames, m_NumberOfVertices, m_NumberOfMaterials;
	std::vector<DirectX::XMFLOAT3> m_Vertices, m_Normals, m_Tangents;
	std::vector<DirectX::XMFLOAT2> m_TextureCoord;
	std::vector<Material> m_Material;
	std::vector<std::vector<IndexDesc>> m_IndexPerMaterial;
	std::vector<std::pair<DirectX::XMFLOAT4, DirectX::XMFLOAT4>> m_WeightsList;
	std::vector<Joint> m_ListOfJoints;
	
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
	const std::vector<DirectX::XMFLOAT3>& getVertices();

	/**
	 * Returns the stored information about indices. 
	 * It returns a list of lists with different index buffers depending on material.
	 * 
	 * @returns a vector of indices.
	 */
	const std::vector<std::vector<ModelConverter::IndexDesc>>& getIndices();

	/**
	 * Returns the stored information about the materials that are used by the model.
	 *
	 * @returns a vector of material structs.
	 */
	const std::vector<ModelConverter::Material>& getMaterial();

	/**
	 * Returns the stored information about normal as a vector with Float3.
	 *
	 * @returns a vector of normals.
	 */
	const std::vector<DirectX::XMFLOAT3>& getNormals();

	/**
	 * Returns the stored information about tangents as a vector with Float3. 
	 *
	 * @returns a vector of tangents.
	 */
	const std::vector<DirectX::XMFLOAT3>& getTangents();

	/**
	 * Returns the stored information about uv coordinates as a vector with Float2. 
	 *
	 * @returns a vector of texture coords.
	 */
	const std::vector<DirectX::XMFLOAT2>& getTextureCoords();

	/**
	 * Returns the number of materials for the test. 
	 *
	 * @returns an int with the correct size of the vector.
	 */
	int getNumberOfMaterial();

	/**
	 * Returns the number of vertices in the vector.
	 *
	 * @returns an int with the correct size of the vector.
	 */
	int getNumberOfVertices();

	/**
	 * Returns a paired vector with information about weights and to what joint it is weighted.
	 *
	 * @returns a paired vector of weights and joints.
	 */
	const std::vector<std::pair<DirectX::XMFLOAT4, DirectX::XMFLOAT4>>& getWeightsList();
	
	/**
	 * Returns a list of joint structs that includes information about animation and parents.
	 *
	 * @return a vector of Joint. 
	 */
	const std::vector<ModelConverter::Joint>& getListOfJoints();

	/**
	 * Returns the start number for the animation. 
	 *
	 * @returns a float with the start value
	 */
	float getAnimationStartValue();

	/**
	 * Returns the end number for the animation. 
	 *
	 * @returns a float with the end value
	 */
	float getAnimationEndValue();

	/**
	 * Returns the number of frames for the animation.
	 *
	 * @returns an in number of frames.
	 */
	int getNumberOfFrames();

	void writeFile(std::string p_FilePath);

	bool loadBinaryFile(std::string p_FilePath);
	
private:
	
	
	void intToByte(int p_Int, std::ofstream* p_Output);
	void floatToByte(float p_Float, std::ofstream* p_Output);
	void stringToByte(std::string p_String, std::ofstream* p_Output);

	int byteToInt(std::ifstream* p_Input);
	float byteToFloat(std::ifstream* p_Input);
	std::string byteToString(int strLength, std::ifstream* p_Input);
	
	ModelConverter::Header readHeader(std::ifstream* p_Input);
	std::vector<ModelConverter::Material> readMaterial(int p_NumberOfMaterial, std::ifstream* p_Input);
	std::vector<ModelConverter::MaterialBuffer> readMaterialBuffer(int p_NumberOfMaterialBuffers, std::ifstream* p_Input);
	std::vector<ModelConverter::VertexBuffer> readVertexBuffer(int p_NumberOfVertex, std::ifstream* p_Input);

	
	
	void createHeader(std::ofstream* p_Output);
	void createMaterial(std::ofstream* p_Output);
	void createMaterialBuffer(std::ofstream* p_Output);
	void createVertexBuffer(std::ofstream* p_Output);
	

	void clearData();
};