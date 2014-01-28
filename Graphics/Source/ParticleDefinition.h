#pragma once

#include "WrapperFactory.h"
#include "ShaderStructs.h"

#include <memory>
#include <vector>
#include <map>

struct Particle
{
	DirectX::XMFLOAT4 Position; //pos from the syspos, in cm
	DirectX::XMFLOAT4 Velocity;
	DirectX::XMFLOAT4 color;
	float sizeX;
	float sizeY;
	float life; //Life for a particle to live before taken away, in s
	float lifeMax;

	Particle(DirectX::XMFLOAT3 p_Position, DirectX::XMFLOAT3 p_Velocity, DirectX::XMFLOAT4 p_Color,
		float p_SizeX, float p_SizeY, float p_Life, float p_LifeMax)
	{
		Position = DirectX::XMFLOAT4(p_Position.x, p_Position.y, p_Position.z, 1.0f);
		Velocity = DirectX::XMFLOAT4(p_Velocity.x, p_Velocity.y, p_Velocity.z, 1.0f);
		color = p_Color;
		sizeX	= p_SizeX;
		sizeY	= p_SizeY;
		life	= p_Life;
		lifeMax = p_LifeMax;
	}
};

/**
 * Representation of the static data associated with a particle system.
 */
struct ParticleDefinition
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
	Shader *shader;

	/**
	 * Material diffuse textures.
	 */
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> diffuseTexture;

	/**
	 * The number of materials in this model.
	 */
	unsigned int numOfMaterials;

	/**
	 * If the model is transparent or not.
	 */
	bool m_IsTransparent;

	/**
	 * Move constructor.
	 */
	ParticleDefinition(ParticleDefinition&& p_Other)
		:	vertexBuffer(std::move(p_Other.vertexBuffer)),
			drawInterval(p_Other.drawInterval),
			shader(p_Other.shader),
			diffuseTexture(p_Other.diffuseTexture),
			numOfMaterials(p_Other.numOfMaterials),
			m_IsTransparent(p_Other.m_IsTransparent)
	{}

	/**
	 * Move assignment operator. Swaps the data of the two objects.
	 */
	ParticleDefinition& operator=(ParticleDefinition&& p_Other)
	{
		std::swap(vertexBuffer, p_Other.vertexBuffer);
		std::swap(drawInterval, p_Other.drawInterval);
		std::swap(shader, p_Other.shader);
		std::swap(diffuseTexture, p_Other.diffuseTexture);
		std::swap(numOfMaterials, p_Other.numOfMaterials);
		std::swap(m_IsTransparent, p_Other.m_IsTransparent);
		
		return *this;
	}

private:
	ParticleDefinition(const ParticleDefinition&); //removed??

public:
	/**
	 * Default contructor. Constructs an object without any data.
	 */
	ParticleDefinition()
		:	shader(nullptr),
			numOfMaterials(0),
			m_IsTransparent(false) {}

	~ParticleDefinition(){}
};

