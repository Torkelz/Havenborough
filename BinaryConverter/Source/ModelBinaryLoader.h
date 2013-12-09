#pragma once

#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>

class ModelBinaryLoader
{
public:
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

	struct Header
	{
		std::string m_modelName;
		int m_numMaterial;
		int m_numVertex;
		int m_numMaterialBuffer;
		int m_numJoints;
		int m_numFrames;
	};

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

private:

public:
	
	/**
	 * Constructor.
	 */
	ModelBinaryLoader();
	
	/**
	 * Destructor.
	 */
	~ModelBinaryLoader();
	
	/**
	 * Use this function to release the memory in loader vectors.
	 */
	void clear();

	bool loadBinaryFile(std::string p_FilePath);
	

private:

	void byteToInt(std::istream* p_Input, int* p_Return);
	void byteToFloat(std::istream* p_Input, float* p_Return);
	void byteToString(std::istream* p_Input, std::string* p_Return);
	void byteToFloat4(std::istream* p_Input, DirectX::XMFLOAT4* p_Return);
	void byteToFloat3(std::istream* p_Input, DirectX::XMFLOAT3* p_Return);
	void byteToFloat2(std::istream* p_Input, DirectX::XMFLOAT2* p_Return);
	
	ModelBinaryLoader::Header readHeader(std::istream* p_Input);
	std::vector<ModelBinaryLoader::Material> readMaterial(int p_NumberOfMaterial, std::istream* p_Input);
	std::vector<ModelBinaryLoader::MaterialBuffer> readMaterialBuffer(int p_NumberOfMaterialBuffers, std::istream* p_Input);
	std::vector<ModelBinaryLoader::VertexBuffer> readVertexBuffer(int p_NumberOfVertex, std::istream* p_Input);	
	std::vector<ModelBinaryLoader::Joint> readJointList(int p_NumberOfJoint, int p_NumberOfFrames, std::istream* p_Input);
	void clearData();
};