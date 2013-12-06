#pragma once
#include "WrapperFactory.h"

struct Model
{
	Buffer *vertexBuffer;
	Buffer **indexBuffer;
	Shader *shader;
	ID3D11ShaderResourceView **diffuseTexture;
	ID3D11ShaderResourceView **normalTexture;
	ID3D11ShaderResourceView **specularTexture;
	unsigned int numOfMaterials;
};