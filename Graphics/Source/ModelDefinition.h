#pragma once
#include "WrapperFactory.h"

#include <memory>
#include <vector>

struct Model
{
	std::unique_ptr<Buffer> vertexBuffer;
	std::vector<std::unique_ptr<Buffer>> indexBuffers;
	Shader *shader;
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> diffuseTexture;
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> normalTexture;
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> specularTexture;
	unsigned int numOfMaterials;

	Model() {};
	Model(Model&& p_Other)
		:	vertexBuffer(std::move(p_Other.vertexBuffer)),
			indexBuffers(std::move(p_Other.indexBuffers)),
			shader(p_Other.shader),
			diffuseTexture(p_Other.diffuseTexture),
			normalTexture(p_Other.normalTexture),
			specularTexture(p_Other.specularTexture),
			numOfMaterials(p_Other.numOfMaterials)
	{}

	Model& operator=(Model&& p_Other)
	{
		std::swap(vertexBuffer, p_Other.vertexBuffer);
		std::swap(indexBuffers, p_Other.indexBuffers);
		std::swap(shader, p_Other.shader);
		std::swap(diffuseTexture, p_Other.diffuseTexture);
		std::swap(normalTexture, p_Other.normalTexture);
		std::swap(specularTexture, p_Other.specularTexture);
		std::swap(numOfMaterials, p_Other.numOfMaterials);

		return *this;
	}

private:
	Model(const Model&);
};