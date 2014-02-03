#pragma once
#include "ShaderStructs.h"
#include "Shader.h"

#include <memory>
#include <string>

struct Particle
{
public:
	ShaderParticle shaderData; //ShaderStruct to define a particle; Position, Color
	DirectX::XMFLOAT4 velocity; //the velocity of a particle, in cm
	DirectX::XMFLOAT2 size; //The size of a particle from edge to edge
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

	/**
	 * Definition about the usage of the texture.
	 */
	ID3D11SamplerState* sampler;

	/**
	 * An identifier to the diffuse texture 
	 */
	std::string textureResourceName;

	/**
	 * How many particles the effect can have active at the same time.
	 */
	unsigned int maxParticles;
	
	/**
	 * How many particle the effect can create on a second.
	 */
	unsigned int particlesPerSec;
	
	/**
	 * How long a particle can live after it was created, in sec.
	 */
	float maxLife;
	
	/**
	 * How big a particle are from edge to edge.
	 */
	DirectX::XMFLOAT2 size; //in cm
	
	/**
	 * An identifier of the particle effect.
	 */
	std::string particleSystemName;
	
	/**
	 * A factor to tell how much from base Position it can change.
	 */
	float particlePositionDeviation;
	
	/**
	 * A factor to tell how much from base velocity it can change.
	 */
	float velocityDeviation;
	
	/**
	 * How much it can go in any direction from the base color.
	 * should not be over 1 or below 0 when added or subtracted with the base color. [0,1]
	 */
	DirectX::XMFLOAT4 particleColorDeviation;

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