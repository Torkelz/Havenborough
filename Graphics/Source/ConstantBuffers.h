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

struct c2D_ObjectBuffer
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
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

struct cSSAO_Buffer
{
	DirectX::XMFLOAT4 corners[4];
	DirectX::XMFLOAT4 offsetVectors[14];
	float occlusionRadius;
	float surfaceEpsilon;
	float occlusionFadeEnd;
	float occlusionFadeStart;
};

struct cSSAO_BlurBuffer
{
	float texelWidth;
	float texelHeight;
	int horizontalBlur;
};