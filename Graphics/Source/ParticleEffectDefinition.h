#pragma once
#include "ShaderStructs.h"
#include "Shader.h"

#include <memory>
#include <string>

struct Particle
{
public:
	ShaderParticle shaderData;
	DirectX::XMFLOAT4 velocity;
	DirectX::XMFLOAT2 size;
	float life; //Life for a particle to live before taken away, in sec

	Particle()
		:	velocity(0.f, 0.f, 0.f, 0.f),
			size(1.f, 1.f),
			life(10.f)
	{
	}

	Particle(DirectX::XMFLOAT3 p_Position, DirectX::XMFLOAT3 p_Velocity, DirectX::XMFLOAT4 p_Color,
		DirectX::XMFLOAT2 p_Size, float p_Life)
	{
		shaderData.position = p_Position;
		velocity = DirectX::XMFLOAT4(p_Velocity.x, p_Velocity.y, p_Velocity.z, 1.0f);
		shaderData.color = p_Color;
		size = p_Size;
		life	= p_Life;
	}
};

/**
 * Representation of the static data associated with a particle system.
 */
struct ParticleEffectDefinition
{
public:
	typedef std::shared_ptr<ParticleEffectDefinition> ptr;

	/**
	 * The shader bound to the model, or nullptr if no shader has been bound.
	 */
	Shader *shader;

	/**
	 * Material diffuse textures.
	 */
	ID3D11ShaderResourceView* diffuseTexture;
	ID3D11SamplerState* sampler;
	std::string textureResourceName;

	unsigned int maxParticles;
	unsigned int particlesPerSec;
	float maxLife;
	DirectX::XMFLOAT2 size; //in cm
	std::string particleSystemName;
	float				particlePositionDeviation; // in cm
	float				velocityDeviation; // in cm/s
	DirectX::XMFLOAT4	particleColorDeviation; // [0,1]

	/**
	 * Default constructor. Constructs an object without any data.
	 */
	ParticleEffectDefinition()
		:	shader(nullptr),
			diffuseTexture(nullptr),
			maxParticles(0),
			maxLife(0),
			size(0.f, 0.f),
			particleSystemName("NO NAME FOUND"),
			particlesPerSec(0),
			particlePositionDeviation(0.f),
			velocityDeviation(0.f),
			particleColorDeviation(0.f, 0.f, 0.f, 0.f)
	{}

	~ParticleEffectDefinition(){}
};