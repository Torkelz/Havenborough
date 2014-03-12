#pragma once
#include "ModelDefinition.h"
#include "../../Common/Source/Utilities/XMFloatUtil.h"
#include <DirectXMath.h>

struct Renderable2D
{
	ModelDefinition *model;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4X4 rotation;
	Vector2 halfSize;
	Vector3 scale;
	Vector4 color;
	
	Renderable2D(ModelDefinition *p_Definition) :
		model(p_Definition), position(DirectX::XMFLOAT3()), rotation(DirectX::XMFLOAT4X4()),
		scale(Vector3(1.0f, 1.0f, 1.0f)), color(Vector4(1.f, 1.f, 1.f, 1.f))
	{
	}

	~Renderable2D(void)
	{
		model = nullptr;
	}

	DirectX::XMFLOAT4X4 getWorldMatrix(void)
	{
		DirectX::XMFLOAT4X4 returnMatrix;
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMLoadFloat4x4(&rotation);

		DirectX::XMStoreFloat4x4(&returnMatrix, DirectX::XMMatrixTranspose(rotationMatrix * scaleMatrix * translationMatrix));
		return returnMatrix;
	}
};
