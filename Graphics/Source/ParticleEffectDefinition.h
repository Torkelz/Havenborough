#pragma once

#include "WrapperFactory.h"
#include "ShaderStructs.h"

#include <memory>
#include <vector>
#include <map>

struct Particle
{
	DirectX::XMFLOAT4 Position; //position in the world, in cm
	DirectX::XMFLOAT4 Velocity;
	DirectX::XMFLOAT4 color;
	float sizeX;
	float sizeY;
	float life; //Life for a particle to live before taken away, in sec

	Particle(DirectX::XMFLOAT3 p_Position, DirectX::XMFLOAT3 p_Velocity, DirectX::XMFLOAT4 p_Color,
		float p_SizeX, float p_SizeY, float p_Life)
	{
		Position = DirectX::XMFLOAT4(p_Position.x, p_Position.y, p_Position.z, 1.0f);
		Velocity = DirectX::XMFLOAT4(p_Velocity.x, p_Velocity.y, p_Velocity.z, 1.0f);
		color = p_Color;
		sizeX	= p_SizeX;
		sizeY	= p_SizeY;
		life	= p_Life;
	}
};

/**
 * Representation of the static data associated with a particle system.
 */
struct ParticleEffectDefinition
{
	/**
	 * The shader bound to the model, or nullptr if no shader has been bound.
	 */
	Shader *shader;

	/**
	 * Material diffuse textures.
	 */
	ID3D11ShaderResourceView* diffuseTexture;

	unsigned int maxParticles;
	unsigned int particlesPerSec;
	float maxLife;
	string particleSystemName;
	DirectX::XMFLOAT3	particlePositionDeviation;
	DirectX::XMFLOAT3	velocityDeviation;
	DirectX::XMFLOAT4	particleColorDeviation;

	/**
	 * Move constructor.
	 */
	ParticleEffectDefinition(ParticleEffectDefinition&& p_Other)
		:	shader(p_Other.shader),
			diffuseTexture(p_Other.diffuseTexture),
			maxParticles(p_Other.maxParticles),
			maxLife(p_Other.maxLife),
			particleSystemName(p_Other.particleSystemName),
			particlesPerSec(p_Other.particlesPerSec),
			particlePositionDeviation(p_Other.particlePositionDeviation),
			velocityDeviation(p_Other.velocityDeviation),
			particleColorDeviation(p_Other.particleColorDeviation)

	{}

	/**
	 * Move assignment operator. Swaps the data of the two objects.
	 */
	ParticleEffectDefinition& operator=(ParticleEffectDefinition&& p_Other)
	{
		std::swap(shader, p_Other.shader);
		std::swap(diffuseTexture, p_Other.diffuseTexture);
		std::swap(maxParticles, p_Other.maxParticles);
		std::swap(maxLife, p_Other.maxLife);
		std::swap(particleSystemName, p_Other.particleSystemName);
		std::swap(particlesPerSec, p_Other.particlesPerSec);
		std::swap(particlePositionDeviation, p_Other.particlePositionDeviation);
		std::swap(velocityDeviation, p_Other.velocityDeviation);
		std::swap(particleColorDeviation, p_Other.particleColorDeviation);

		return *this;
	}

private:
	ParticleEffectDefinition(const ParticleEffectDefinition&); //removed??

public:
	/**
	 * Default constructor. Constructs an object without any data.
	 */
	ParticleEffectDefinition()
		:	shader(nullptr),
			diffuseTexture(nullptr),
			maxParticles(0),
			maxLife(0),
			particleSystemName("NO NAME FOUND"),
			particlesPerSec(0),
			particlePositionDeviation(0.f, 0.f, 0.f),
			velocityDeviation(0.f, 0.f, 0.f),
			particleColorDeviation(0.f, 0.f, 0.f, 0.f)
	{}

	~ParticleEffectDefinition(){}
};

