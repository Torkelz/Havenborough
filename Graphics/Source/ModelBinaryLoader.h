#pragma once

#include "ShaderStructs.h"

#include <array>
#include <fstream>
#include <vector>
#include <string>

class ModelBinaryLoader
{
public:
	struct Header
	{
		std::string m_ModelName;
		int m_NumMaterial;
		int m_NumVertex;
		int m_NumMaterialBuffer;
		bool m_Animated;
		bool m_Transparent;
		bool m_CollideAble;
	};

private:
	Header m_FileHeader;
	std::vector<Material> m_Material;
	std::vector<AnimatedVertex> m_AnimationVertexBuffer;
	std::vector<StaticVertex> m_VertexBuffer;
	std::vector<MaterialBuffer> m_MaterialBuffer;
	std::array<DirectX::XMFLOAT3, 8> m_BoundingVolume;

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
	void loadBinaryFile(std::string p_FilePath);
	
	/**
	 * Returns information about the materials used in the model.
	 *
	 * @returns a vector of the struct Material.
	 */
	const std::vector<Material>& getMaterial() const;

	/**
	 * Returns information about animated vertices in form of a vertexbuffer.
	 *
	 * @returns a vector of the struct VertexAnimation.
	 */
	const std::vector<AnimatedVertex>& getAnimatedVertexBuffer() const;

	/**
	 * Returns information about vertices. This function does not return animated vertices.
	 * Use getAnimatedVertexBuffer() to get information about animated vertices.
	 *
	 * @returns a vector of the struct Vertex.
	 */
	const std::vector<StaticVertex>& getStaticVertexBuffer() const;

	/**
	 * Returns information about what material is used on a part of the model.
	 *
	 * @returns a vector of the struct MaterialBuffer.
	 */
	const std::vector<MaterialBuffer>& getMaterialBuffer() const;

	/**
	 * Returns a true or fasle about if the model is animated.
	 *
	 * @returns a bool;
	 */
	bool getAnimated() const;

	/**
	 * Returns a true or fasle about if the model is transparent.
	 *
	 * @returns a bool;
	 */
	bool getTransparent() const;

	/**
	 * Returns a true or fasle about if the model is collide-able.
	 *
	 * @returns a bool;
	 */
	bool getCollideAble() const;

	const std::array<DirectX::XMFLOAT3, 8>& getBoundingVolume() const;

protected:
	void byteToInt(std::istream* p_Input, int& p_Return);
	void byteToString(std::istream* p_Input, std::string& p_Return);
	
	ModelBinaryLoader::Header readHeader(std::istream* p_Input);
	std::vector<Material> readMaterial(int p_NumberOfMaterial, std::istream* p_Input);
	std::vector<MaterialBuffer> readMaterialBuffer(int p_NumberOfMaterialBuffers, std::istream* p_Input);
	std::vector<StaticVertex> readVertexBuffer(int p_NumberOfVertex, std::istream* p_Input);
	std::vector<AnimatedVertex> readVertexBufferAnimation(int p_NumberOfVertex, std::istream* p_Input);	

private:
	void clearData();

	void calculateBoundingVolume(const std::vector<AnimatedVertex>& p_Vertices);
	void calculateBoundingVolume(const std::vector<StaticVertex>& p_Vertices);
};