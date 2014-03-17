#pragma pack_matrix(row_major)
#include "DeferredHelper.hlsl"

SamplerState textureSampler	: register(s0);
Texture2D diffuseTex		: register(t0);
Texture2D normalTex			: register(t1);

cbuffer cb : register(b0)
{
	float4x4 cView;
	float4x4 cProjection;
	float3	 cCameraPos;
	float		cSSAOScale;
};

cbuffer cbWorld : register(b1)
{
	float4x4 cWorld;
};

struct VSIn
{
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 uvCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
	float4x4 vworld	: WORLD;
};

//############################
// Shader step: Vertex Shader
//############################
PSIn VS( VSIn input )
{
	PSIn output;

	output.position = mul( cProjection, mul(cView, mul(input.vworld, input.pos)));
	output.wposition = mul(input.vworld, input.pos);

	output.normal = normalize(mul(input.vworld, float4(input.normal, 0.f)).xyz);
	output.uvCoord = input.uvCoord;
	output.tangent = normalize(mul(input.vworld, float4(input.tangent, 0.f)).xyz);
	output.binormal = normalize(mul(input.vworld, float4(input.binormal, 0.f)).xyz);
	output.depth = mul(cView, mul(input.vworld, input.pos)).z;
			
	return output;
}

//############################
// Shader step: Pixel Shader
//############################
PSOut PS( PSIn input )
{
	return PSFunction(input, cView, diffuseTex, normalTex, textureSampler);
}