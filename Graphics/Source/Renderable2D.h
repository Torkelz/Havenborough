#pragma once
#include "ModelDefinition.h"

struct Renderable2D
{
	ModelDefinition *model;
	
	Renderable2D(ModelDefinition *p_Definition) :
		model(p_Definition)
	{

	}

	~Renderable2D(void)
	{
		model = nullptr;
	}
};
