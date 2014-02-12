#pragma once
#include "ModelDefinition.h"
#include "ParticleInstance.h"

struct Renderable
{
	enum class Type
	{
		DEFERRED_OBJECT,
		FORWARD_OBJECT,
		PARTICLE_SYSTEM,
	};

	Type type;
	ModelDefinition *model;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 invTransposeWorld;
	const std::vector<DirectX::XMFLOAT4X4> *finalTransforms;
	const DirectX::XMFLOAT3 *colorTone;
	ParticleInstance::ptr particles;

	Renderable(Type p_Type, ModelDefinition *p_Model, const DirectX::XMFLOAT4X4& p_World,
		const std::vector<DirectX::XMFLOAT4X4>* p_FinalTransforms = nullptr, 
		const DirectX::XMFLOAT3 *p_ColorTone = nullptr)
	{
		using namespace DirectX;

		type = p_Type;
		model = p_Model;
		world = p_World;
		colorTone = p_ColorTone;

		XMStoreFloat4x4(&invTransposeWorld, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&world)))); 
		invTransposeWorld._41 = 0.f;
		invTransposeWorld._42 = 0.f;
		invTransposeWorld._43 = 0.f;
		invTransposeWorld._44 = 1.f;

		finalTransforms = p_FinalTransforms;
	}

	Renderable(ParticleInstance::ptr p_Particles)
		:	type(Type::PARTICLE_SYSTEM),
			model(nullptr),
			particles(p_Particles),
			world(p_Particles->getWorldMatrix()),
			invTransposeWorld(),
			finalTransforms(nullptr),
			colorTone(nullptr)
	{
	}

	~Renderable()
	{
		model = nullptr;
	}
};