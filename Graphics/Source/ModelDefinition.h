#pragma once

#include "Joint.h"
#include "ShaderStructs.h"
#include "WrapperFactory.h"
#include "AnimationStructs.h"

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
	/**
	 * The vertex range for each material.
	 */
	std::vector<std::pair<int,int>> drawInterval;
	/**
	 * The shader bound to the model, or nullptr if no shader has been bound.
	 */
	Shader *shader;
	/**
	 * Material diffuse textures.
	 */
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> diffuseTexture;
	/**
	 * Material normal textures.
	 */
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> normalTexture;
	/**
	 * Material specular textures.
	 */
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> specularTexture;
	/**
	 * The number of materials in this model.
	 */
	unsigned int numOfMaterials;
	/**
	 * If the model is animated or static.
	 */
	bool isAnimated;
	/**
	 * If the model is transparent or not.
	 */
	bool isTransparent;

	/**
	 * The animation joints of the model. Contains all static animation data.
	 */
	std::vector<Joint> joints;

	/**
	 * The animation clips. Address them via a name. E.g. "Walk", "Run", "Laugh"...
	 */
	std::map<std::string, AnimationClip> animationClips;

public:
	/**
	 * Default contructor. Constructs an object without any data.
	 */
	ModelDefinition()
		:	shader(nullptr),
			numOfMaterials(0),
			isAnimated(false),
			isTransparent(false) {}

	~ModelDefinition(){}
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
			isAnimated(p_Other.isAnimated),
			isTransparent(p_Other.isTransparent),
			joints(std::move(p_Other.joints)),
			animationClips(std::move(p_Other.animationClips))
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
		std::swap(isAnimated, p_Other.isAnimated);
		std::swap(isTransparent, p_Other.isTransparent);
		std::swap(joints, p_Other.joints);
		std::swap(animationClips, p_Other.animationClips);

		return *this;
	}

private:
	ModelDefinition(const ModelDefinition&); //Should never be used, but must be private!
};