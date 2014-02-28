#pragma pack_matrix(row_major)
#include "LightHelper.hlsl"

Texture2D diffuseTex : register (t0);

float4 VS(float3 pos : POSITION) : SV_POSITION
{
	return float4(pos, 1.f);
}

float4 PS(float4 pos : SV_POSITION) : SV_TARGET
{
	return float4(GetGBufferDiffuseSpec(int3(pos.xy, 0), diffuseTex).xyz * 0.05f, 1.f);
}