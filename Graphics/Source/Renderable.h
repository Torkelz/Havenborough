#pragma once
#include "ModelDefinition.h"

struct Renderable
{
	ModelDefinition *model;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 invTransposeWorld;
	const std::vector<DirectX::XMFLOAT4X4> *finalTransforms;
	const DirectX::XMFLOAT3 *colorTone;

	Renderable(ModelDefinition *p_Model, const DirectX::XMFLOAT4X4& p_World,
		const std::vector<DirectX::XMFLOAT4X4>* p_FinalTransforms = nullptr, 
		const DirectX::XMFLOAT3 *p_ColorTone = nullptr)
	{
		using namespace DirectX;

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

	~Renderable()
	{
		model = nullptr;
	}
};