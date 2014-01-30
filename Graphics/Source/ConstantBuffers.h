#pragma once
#include <DirectXMath.h>
/*
* cBuffer contains the matrices needed to render the models and lights.
*/
struct cBuffer
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT3	campos;
};

struct cObjectBuffer
{
	DirectX::XMFLOAT4X4 world;
};

struct cAnimatedObjectBuffer
{
	DirectX::XMFLOAT4X4 invTransposeWorld;
	DirectX::XMFLOAT4X4 boneTransform[96];
};