#pragma once
#include "Joint.h"
#include "ShaderStructs.h"

#include <fstream>
#include <vector>
#include <string>

class ModelBinaryLoader
{
public:
	struct Header
	{
		std::string m_modelName;
		int m_numMaterial;
		int m_numVertex;
		int m_numMaterialBuffer;
		int m_numJoints;
		int m_numFrames;
	};

private:
	Header m_FileHeader;
	std::vector<Material> m_Material;
	std::vector<AnimatedVertex> m_AnimationVertexBuffer;
	std::vector<Joint> m_Joints;
	std::vector<StaticVertex> m_VertexBuffer;
	std::vector<MaterialBuffer> m_MaterialBuffer;

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

	/**
	 * Opens a binary file then reads the information stream and saves the information in vectors of structs.
	 * 
	 * @param p_FilePath, the absolute path to the source file.
	 *
	 */
	bool loadBinaryFile(std::string p_FilePath);
	
	/**
	 * Returns information about the materials used in the model.
	 *
	 * @returns a vector of the struct Material.
	 */
	const std::vector<Material>& getMaterial();

	/**
	 * Returns information about animated vertices in form of a vertexbuffer.
	 *
	 * @returns a vector of the struct VertexAnimation.
	 */
	const std::vector<AnimatedVertex>& getAnimatedVertexBuffer();

	/**
	 * Returns a vector of joints for the animation. 
	 ** This will be empty if the source file does not contain any animation.
	 *
	 * @returns a vector of the struct Joint.
	 */
	const std::vector<Joint>& getJoints();

	/**
	 * Returns information about vertices. This function does not return animated vertices.
	 * Use getAnimatedVertexBuffer() to get information about animated vertices.
	 *
	 * @returns a vector of the struct Vertex.
	 */
	const std::vector<StaticVertex>& getStaticVertexBuffer();

	/**
	 * Returns information about what material is used on a part of the model.
	 *
	 * @returns a vector of the struct MaterialBuffer.
	 */
	const std::vector<MaterialBuffer>& getMaterialBuffer();

protected:
	void byteToInt(std::istream* p_Input, int& p_Return);
	void byteToString(std::istream* p_Input, std::string& p_Return);
	
	ModelBinaryLoader::Header readHeader(std::istream* p_Input);
	std::vector<Material> readMaterial(int p_NumberOfMaterial, std::istream* p_Input);
	std::vector<MaterialBuffer> readMaterialBuffer(int p_NumberOfMaterialBuffers, std::istream* p_Input);
	std::vector<StaticVertex> readVertexBuffer(int p_NumberOfVertex, std::istream* p_Input);
	std::vector<AnimatedVertex> readVertexBufferAnimation(int p_NumberOfVertex, std::istream* p_Input);	
	std::vector<Joint> readJointList(int p_NumberOfJoint, int p_NumberOfFrames, std::istream* p_Input);

private:
	void clearData();
};