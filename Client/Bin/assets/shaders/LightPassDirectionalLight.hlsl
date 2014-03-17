#pragma pack_matrix(row_major)
#include "LightHelper.hlsl"

#ifndef SHADOW_RES
#define SHADOW_RES 1024
#endif
#ifndef SHADOW_BORDER
#define SHADOW_BORDER 0
#endif

Texture2D wPosTex	 : register (t0);
Texture2D normalTex	 : register (t1);
Texture2D diffuseTex : register (t2);
Texture2D SSAO_Tex	 : register (t3);
Texture2D ShadowMap  : register (t4);

SamplerComparisonState shadowMapSampler : register (s0);

float3 CalcLighting( float3 normal, float3 position, float3 diffuseAlbedo,
	float3 lightPos, float3 lightDirection, float3 lightColor, float3 ssao, float4 light);
float CalcShadowFactor(float3 uv, float nDotL);
float Blur(float3 uv);

cbuffer cb : register(b0)
{
	float4x4	view;
	float4x4	projection;
	float3		cameraPos;
	float		ssaoScale;
};

cbuffer lightMat : register(b1)
{
	float4x4	lightView;
	float4x4	lightProjection;
	int		big;
	int		shadowMapped;
}

//############################
// Shader step: Vertex Shader
//############################
VSLightOutput DirectionalLightVS(VSLightInput input)
{
	VSLightOutput output;
	output.vposition		= float4(input.vposition,1.0f);
	output.lightPos			= input.lightPos;
	output.lightColor		= input.lightColor;
	output.lightDirection	= input.lightDirection;
	output.spotlightAngles	= input.spotlightAngles;
	output.lightRange		= input.lightRange;
	output.lightIntensity	= input.lightIntensity;
	return output;
}

//############################
// Shader step: Pixel Shader
//############################
float4 DirectionalLightPS(VSLightOutput input) : SV_TARGET
{
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 ssao;
	
	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes(input.vposition.xy, ssaoScale, normalTex, diffuseTex, SSAO_Tex, wPosTex,
		normal, diffuseAlbedo, ssao, position);
	
	float4x4 t =
	{
		float4(0.5f, 0.0f, 0.0f, 0.5f),
		float4(0.0f, -0.5f, 0.0f, 0.5f),
		float4(0.0f, 0.0f, 1.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
	};

	float4 lightPos = mul(t, mul(lightProjection, mul(lightView, float4(position, 1.0f))));
	lightPos.xyz /= lightPos.w;
	lightPos.z *= 0.9999888f;
	lightPos.z += -0.000000055f;

	
	float3 lighting = 0;
	float percentage = 0.10f;
	float2 center = float2(0.5f, 0.5f);
	float2 gradient = center - lightPos.xy;
	float innerBorder = SHADOW_BORDER * (1.f + percentage);
	float outerBorder = 1 - SHADOW_BORDER * (1.f + percentage);
	float innerGradBorder = SHADOW_BORDER;
	float outerGradBorder = 1 - SHADOW_BORDER;
	if(shadowMapped == 0)
	{
		lighting = CalcLighting(normal, position, diffuseAlbedo,
				input.lightPos, input.lightDirection, input.lightColor, ssao, lightPos);
	}
	else if(big == 0)
	{
		if(lightPos.x > 0.f && lightPos.x < 1.f && lightPos.y > 0.f && lightPos.y < 1.f)
		{
			lighting = CalcLighting(normal, position, diffuseAlbedo,
				input.lightPos, input.lightDirection, input.lightColor, ssao, lightPos);

			gradient = abs(gradient);
			gradient = gradient - (0.5f - percentage);
			gradient = gradient/percentage;
			gradient = clamp(gradient, 0, 1);

			lighting = lighting * (1 - gradient.x) * (1 - gradient.y);
		}
	}
	else
	{
		if(lightPos.x < innerBorder || lightPos.x > outerBorder || lightPos.y < innerBorder || lightPos.y > outerBorder)
		{
			lighting = CalcLighting(normal, position, diffuseAlbedo,
				input.lightPos, input.lightDirection, input.lightColor, ssao, lightPos);

			gradient = abs(gradient);
			gradient = gradient - ((1-2*SHADOW_BORDER)*(0.5f-percentage));
			gradient = gradient/((1-2*SHADOW_BORDER)*percentage);
			gradient = clamp(gradient, 0, 1);

			lighting = lighting * (1 - (1 - gradient.x) * (1 - gradient.y));
		}
	}
	return float4( lighting * input.lightIntensity, 1.0f );
}


//################################
//		HELPER FUNCTIONS
//################################
float3 CalcLighting(float3 normal, float3 position,	float3 diffuseAlbedo,
		float3 lightPos, float3 lightDirection, float3 lightColor, float3 ssao, float4 light)
{

	float attenuation = 1.0f;
	float3 L = -lightDirection;
	L = mul(view, float4(L, 0.f)).xyz;

	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * lightColor * diffuseAlbedo * pow(ssao, 10);
	
	diffuse *=  CalcShadowFactor(light.xyz, nDotL);

	// Final value is the sum of the albedo and diffuse with attenuation applied
	return saturate(diffuse * attenuation);
}

//texel size
static const float SMAP_SIZE = SHADOW_RES;
static const float SMAP_DX = 1.0f / SMAP_SIZE;



float CalcShadowFactor(float3 uv, float nDotL)
{
	float percentLit = 1.0f;
	 if ((saturate(uv.x) == uv.x) &&
        (saturate(uv.y) == uv.y) && uv.z > 0)
    {
		float value = 0.f;
		float coefficients[25] = 
		{
			0.003663,0.014652,0.025641,0.014652,0.003663,
			0.014652,0.058608,0.095238,0.058608,0.014652,
			0.025641,0.095238,0.150183,0.095238,0.025641,
			0.014652,0.058608,0.095238,0.058608,0.014652,
			0.003663,0.014652,0.025641,0.014652,0.003663
		};

		[unroll]
		for(int i = 0; i < 5; i++)
		{
			for(int j = 0; j < 5; j++)
			{
				value += ShadowMap.SampleCmpLevelZero(shadowMapSampler, float2(uv.x + (i - 2) * SMAP_DX, uv.y + (j - 2) * SMAP_DX), uv.z) * coefficients[i * 5 + j];
			}
		}
		percentLit = value;
	 }
	return percentLit;

}