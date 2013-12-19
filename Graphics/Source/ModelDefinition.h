#pragma once

#include "Joint.h"
#include "ShaderStructs.h"
#include "WrapperFactory.h"

#include <memory>
#include <vector>

/**
 * Representation of the static data associated with a model.
 */
struct ModelDefinition
{
	/**
	 * The GPU buffer containing the vertex data.
	 */
	std::unique_ptr<Buffer> vertexBuffer;
	/**
	 * The vertex range for each material.
	 */
	std::vector<std::pair<int,int>> drawInterval;
	/**
	 * The shader bound to the model, or nullptr if no shader has been bound.
	 */
	Shader						*shader;
	/**
	 * Matrial diffuse textures.
	 */
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> diffuseTexture;
	/**
	 * Matrial normal textures.
	 */
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> normalTexture;
	/**
	 * Matrial specular textures.
	 */
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> specularTexture;
	/**
	 * The number of materials in this model.
	 */
	unsigned int				numOfMaterials;
	/**
	 * If the model is animated or static.
	 */
	bool						m_IsAnimated;

	/**
	 * The animation joints of the model. Contains all static animation data.
	 */
	std::vector<Joint>			m_Joints;

	/**
	 * Move constructor.
	 */
	ModelDefinition(ModelDefinition&& p_Other)
		:	vertexBuffer(std::move(p_Other.vertexBuffer)),
			drawInterval(p_Other.drawInterval),
			shader(p_Other.shader),
			diffuseTexture(p_Other.diffuseTexture),
			normalTexture(p_Other.normalTexture),
			specularTexture(p_Other.specularTexture),
			numOfMaterials(p_Other.numOfMaterials),
			m_IsAnimated(p_Other.m_IsAnimated),
			m_Joints(std::move(p_Other.m_Joints))
	{}

	/**
	 * Move assignment operator. Swaps the data of the two objects.
	 */
	ModelDefinition& operator=(ModelDefinition&& p_Other)
	{
		std::swap(vertexBuffer, p_Other.vertexBuffer);
		std::swap(drawInterval, p_Other.drawInterval);
		std::swap(shader, p_Other.shader);
		std::swap(diffuseTexture, p_Other.diffuseTexture);
		std::swap(normalTexture, p_Other.normalTexture);
		std::swap(specularTexture, p_Other.specularTexture);
		std::swap(numOfMaterials, p_Other.numOfMaterials);
		std::swap(m_IsAnimated, p_Other.m_IsAnimated);
		std::swap(m_Joints, p_Other.m_Joints);

		return *this;
	}

private:
	ModelDefinition(const ModelDefinition&); // Removed

public:
	/**
	 * Default contructor. Constructs an object without any data.
	 */
	ModelDefinition()
		:	shader(nullptr),
			numOfMaterials(0),
			m_IsAnimated(false) {}
	~ModelDefinition(){}
};