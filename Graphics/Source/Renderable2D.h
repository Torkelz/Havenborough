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
};
