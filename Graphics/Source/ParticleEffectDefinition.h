#pragma once
#include "ShaderStructs.h"
#include "Shader.h"

#include <memory>
#include <string>

struct Particle
{
public:
	DirectX::XMFLOAT4 position; //position in the world, in cm
	DirectX::XMFLOAT4 velocity;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 size;
	float life; //Life for a particle to live before taken away, in sec

	Particle()
		:	position(0.f, 0.f, 0.f, 1.f),
		velocity(0.f, 0.f, 0.f, 0.f),
		color(1.f, 0.f, 1.f, 1.f),
		size(1.f, 1.f),
		life(10.f)
	{
	}

	Particle(DirectX::XMFLOAT3 p_Position, DirectX::XMFLOAT3 p_Velocity, DirectX::XMFLOAT4 p_Color,
		DirectX::XMFLOAT2 p_Size, float p_Life)
	{
		position = DirectX::XMFLOAT4(p_Position.x, p_Position.y, p_Position.z, 1.0f);
		velocity = DirectX::XMFLOAT4(p_Velocity.x, p_Velocity.y, p_Velocity.z, 1.0f);
		color = p_Color;
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
	std::string textureResourceName;

	unsigned int maxParticles;
	unsigned int particlesPerSec;
	float maxLife;
	DirectX::XMFLOAT2 size; //in cm
	std::string particleSystemName;
	DirectX::XMFLOAT3	particlePositionDeviation; // in cm
	DirectX::XMFLOAT3	velocityDeviation; // in cm/s
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
		particlePositionDeviation(0.f, 0.f, 0.f),
		velocityDeviation(0.f, 0.f, 0.f),
		particleColorDeviation(0.f, 0.f, 0.f, 0.f)
	{}

	~ParticleEffectDefinition(){}
};