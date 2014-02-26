#pragma pack_matrix(row_major)
#include "LightHelper.hlsl"

#ifndef SHADOW_RES
#define SHADOW_RES 1024
#endif

Texture2D wPosTex	 : register (t0);
Texture2D normalTex	 : register (t1);
Texture2D diffuseTex : register (t2);
Texture2D SSAO_Tex	 : register (t3);
Texture2D ShadowMap  : register (t4);

SamplerComparisonState shadowMapSampler : register (s0);

float3 CalcLighting( float3 normal, float3 position, float3 diffuseAlbedo, float3 specularAlbedo,
	float specularPower, float3 lightPos, float3 lightDirection, float3 lightColor, float3 ssao, float blurPercentage, float calcPercentage);
float CalcShadowFactor(float3 uv);
float Blur(float3 uv);

cbuffer cb : register(b0)
{
	float4x4	view;
	float4x4	projection;
	float3		cameraPos;
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
	float3 specularAlbedo;
	float specularPower;
	float3 ssao;
	
	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes(input.vposition.xy, normalTex, diffuseTex, SSAO_Tex, wPosTex,
		normal, diffuseAlbedo, specularAlbedo, ssao, position, specularPower);
	
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
	if(shadowMapped == 0)
	{
		lighting = CalcLighting(normal, position, diffuseAlbedo, specularAlbedo, 
				specularPower,input.lightPos, input.lightDirection, input.lightColor, ssao, 0, 0);
	}
	else if(big == 0)
	{
		if(lightPos.x > 0.f && lightPos.x < 1.f && lightPos.y > 0.f && lightPos.y < 1.f)
		{
			lighting = CalcLighting(normal, position, diffuseAlbedo, specularAlbedo, 
				specularPower,input.lightPos, input.lightDirection, input.lightColor, ssao, Blur(lightPos.xyz), CalcShadowFactor(lightPos.xyz));
		}
	}
	else
	{
		if(lightPos.x < 0.4f || lightPos.x > 0.6f || lightPos.y < 0.4f || lightPos.y > 0.6f)
		{
			lighting = CalcLighting(normal, position, diffuseAlbedo, specularAlbedo, 
				specularPower,input.lightPos, input.lightDirection, input.lightColor, ssao, Blur(lightPos.xyz), CalcShadowFactor(lightPos.xyz));
		}
	}
	return float4( lighting * input.lightIntensity, 1.0f );
}


//################################
//		HELPER FUNCTIONS
//################################
float3 CalcLighting(float3 normal, float3 position,	float3 diffuseAlbedo, float3 specularAlbedo,
	float specularPower, float3 lightPos, float3 lightDirection, float3 lightColor, float3 ssao, float blurPercentage, float calcPercentage)
{

	float attenuation = 1.0f;
	float3 L = -lightDirection;
	L = mul(view, float4(L, 0.f)).xyz;
	
	//if (dot(mul((float3x3)view, float3(0.f, 0.f, 1.f)), normal) < 0.f) return float3(0.f, 0.f, 1.f);

	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * lightColor * diffuseAlbedo * pow(ssao, 10);
	if(shadowMapped != 0)
		diffuse *=  blurPercentage * calcPercentage;

	// Calculate the specular term
	float3 V = normalize(cameraPos - position);
	V = mul(view, float4(V, 0.f)).xyz;

	float3 H = normalize( L + V );
	float3 specular = pow( saturate( dot(normal, H) ), specularPower ) *
							 lightColor * specularAlbedo.xyz * nDotL;
	// Final value is the sum of the albedo and diffuse with attenuation applied

	return saturate(( diffuse + specular) * attenuation);
}

//texel size
static const float SMAP_SIZE = SHADOW_RES;
static const float SMAP_DX = 1.0f / SMAP_SIZE;



float CalcShadowFactor(float3 uv)
{
	float percentLit = 0.0f;

	float value = 0.f;
	float coefficients[21] = 
	{
		0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
		0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
		0.14107424,
		0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
		0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337
	};

	[unroll]
	for(int i = 0; i < 21; i++)
	{
		value += ShadowMap.SampleCmpLevelZero(shadowMapSampler, float2(uv.x + (i - 10) * SMAP_DX, uv.y  + (i - 10) * SMAP_DX), uv.z) * coefficients[i];
	}
	percentLit = value;

	return percentLit;

}


#define SAMPLE_COUNT 3
float2 Offsets[SAMPLE_COUNT];

float log_space(float w0, float d1, float w1, float d2)
{
	return (d1 + log(w0 + (w1 * exp(d2-d1))));
}

float Blur(float3 uv)
{
	float v, B, B2;
	float w = (1.0/SAMPLE_COUNT);
	B = ShadowMap.SampleCmpLevelZero(shadowMapSampler, uv.xy + Offsets[0], uv.z);
	B2 = ShadowMap.SampleCmpLevelZero(shadowMapSampler, uv.xy + Offsets[1], uv.z);
	v = log_space(w, B, w, B2);

	for (int i = 2; i < SAMPLE_COUNT; i++)
	{
		B = ShadowMap.SampleCmpLevelZero(shadowMapSampler, uv.xy + Offsets[i], uv.z);
		v = log_space(1.0, v, w, B);
	}

	return v;
}