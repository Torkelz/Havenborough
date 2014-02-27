#pragma pack_matrix(row_major)
#include "DeferredHelper.hlsl"

SamplerState textureSampler	: register(s0);
Texture2D diffuseTex		: register(t0);
Texture2D normalTex			: register(t1);
Texture2D specularTex		: register(t2);

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
	float4 position	: POSITION;
	float3 normal	: NORMAL;
	float2 uvCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
};

//############################
// Shader step: Vertex Shader
//############################
PSIn VS(VSIn p_Input)
{
	PSIn output;

	output.position = mul(cProjection, mul(cView, mul(cWorld, p_Input.position)));
	output.wposition = mul(cWorld, p_Input.position);

	output.normal = normalize(mul(cWorld, float4(p_Input.normal, 0.f)).xyz);
	output.uvCoord = p_Input.uvCoord;
	output.tangent = normalize(mul(cWorld, float4(p_Input.tangent, 0.f)).xyz);
	output.binormal = normalize(mul(cWorld, float4(p_Input.binormal, 0.f)).xyz);
	output.depth = mul(cView, mul(cWorld, p_Input.position)).z;

	return output;
}

//############################
// Shader step: Pixel Shader
//############################
PSOut PS(PSIn p_Input)
{
	return PSFunction(p_Input, cView, diffuseTex, normalTex, specularTex, textureSampler);
}