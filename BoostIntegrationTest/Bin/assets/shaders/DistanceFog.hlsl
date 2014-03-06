#pragma pack_matrix(row_major)
#include "LightHelper.hlsl"

Texture2D gDistanceTex : register(t0);
Texture2D gColorTex : register(t1);

#ifndef FOG_COLOR
#define FOG_COLOR float4(0.3f, 0.3f, 0.45f, 1.0f)
#endif
#ifndef MIN_DISTANCE
#define MIN_DISTANCE 3000.0f
#endif
#ifndef MAX_DISTANCE
#define MAX_DISTANCE 20000.0f
#endif

cbuffer cb : register(b0)
{
	float4x4	cViewMatrix;
	float4x4	cProjectionMatrix;
	float3		cCameraPosition;
	float		cSSAO_Scale;
}

float4 DistanceFogVS(float3 screenCoord : POSITION) : SV_POSITION
{
	return float4(screenCoord, 1.0f);
}

float4 DistanceFogPS(float4 input : SV_POSITION) : SV_TARGET
{
	int3 sampleIndex = int3(input.xy, 0);
	
	float4 position = float4(GetWorldPosition(sampleIndex, gDistanceTex).xyz, 1.0f);
	position = mul(cViewMatrix, position);
	float distance = position.z;

	float4 color = gColorTex.Load(sampleIndex);

	//Get a fog factor (thickness of fog) based on the distance
	float fogFactor = (distance - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE);
	fogFactor = saturate(fogFactor);
	
	color = lerp(color, FOG_COLOR, fogFactor);

	return color;
}