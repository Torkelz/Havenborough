#pragma once
#include "Buffer.h"
#include "Shader.h"

#include <memory>
#include <vector>
#include <map>

/**
 * Representation of the static data associated with a model.
 */
struct ModelDefinition
{
public:
	/**
	 * The GPU buffer containing the vertex data.
	 */
	std::unique_ptr<Buffer> vertexBuffer;
	
	struct Material
	{
		int vertexStart;
		int numOfVertices;
		int textureIndex;

		Material()
		{
		}

		Material(int p_VertexStart, int p_NumOfVertice, int p_TexureIndex) :
			vertexStart(p_VertexStart),
			numOfVertices(p_NumOfVertice),
			textureIndex(p_TexureIndex)
		{
		}
	};
	
	/**
	 * Sets of materials describing different styles.
	 */
	std::vector<std::pair<std::string, std::vector<Material>>> materialSets;
	/**
	 * The shader bound to the model, or nullptr if no shader has been bound.
	 */
	Shader *shader;
	/**
	 * Material diffuse textures.
	 */
	std::vector<std::pair<std::string, ID3D11ShaderResourceView*>> diffuseTexture;
	/**
	 * Material normal textures.
	 */
	std::vector<std::pair<std::string, ID3D11ShaderResourceView*>> normalTexture;
	/**
	 * Material specular textures.
	 */
	std::vector<std::pair<std::string, ID3D11ShaderResourceView*>> specularTexture;
	/**
	 * If the model is animated or static.
	 */
	bool isAnimated;
	/**
	 * If the model is transparent or not.
	 */
	bool isTransparent;
	/**
	 * If the model is a 2D texture only.
	 */
	bool is2D;

public:
	/**
	 * Default contructor. Constructs an object without any data.
	 */
	ModelDefinition()
		:	shader(nullptr),
			isAnimated(false),
			isTransparent(false),
			is2D(false)
	{}

	~ModelDefinition(){}
	/**
	 * Move constructor.
	 */
	ModelDefinition(ModelDefinition&& p_Other)
		:	vertexBuffer(std::move(p_Other.vertexBuffer)),
			materialSets(p_Other.materialSets),
			shader(p_Other.shader),
			diffuseTexture(p_Other.diffuseTexture),
			normalTexture(p_Other.normalTexture),
			specularTexture(p_Other.specularTexture),
			isAnimated(p_Other.isAnimated),
			isTransparent(p_Other.isTransparent),
			is2D(p_Other.is2D)
	{}

	/**
	 * Move assignment operator. Swaps the data of the two objects.
	 */
	ModelDefinition& operator=(ModelDefinition&& p_Other)
	{
		std::swap(vertexBuffer, p_Other.vertexBuffer);
		std::swap(materialSets, p_Other.materialSets);
		std::swap(shader, p_Other.shader);
		std::swap(diffuseTexture, p_Other.diffuseTexture);
		std::swap(normalTexture, p_Other.normalTexture);
		std::swap(specularTexture, p_Other.specularTexture);
		std::swap(isAnimated, p_Other.isAnimated);
		std::swap(isTransparent, p_Other.isTransparent);
		std::swap(is2D, p_Other.is2D);

		return *this;
	}

private:
	ModelDefinition(const ModelDefinition&); //Should never be used, but must exist!
};