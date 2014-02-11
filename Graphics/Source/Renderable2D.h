#pragma once
#include "ModelDefinition.h"

struct Renderable2D
{
	ModelDefinition *model;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4X4 rotation;
	float scale;
	
	Renderable2D(ModelDefinition *p_Definition) :
		model(p_Definition), position(DirectX::XMFLOAT3()), rotation(DirectX::XMFLOAT4X4()), scale(1.0f)
	{
	}

	~Renderable2D(void)
	{
		model = nullptr;
	}

	DirectX::XMFLOAT4X4 getWorldMatrix(void)
	{
		DirectX::XMFLOAT4X4 returnMatrix;
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMLoadFloat4x4(&rotation);

		DirectX::XMStoreFloat4x4(&returnMatrix, DirectX::XMMatrixTranspose(scaleMatrix * rotationMatrix * translationMatrix));
		return returnMatrix;
	}
};
