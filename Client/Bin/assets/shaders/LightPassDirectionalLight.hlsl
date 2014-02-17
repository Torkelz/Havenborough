#pragma pack_matrix(row_major)
#include "LightHelper.hlsl"

Texture2D wPosTex	 : register (t0);
Texture2D normalTex	 : register (t1);
Texture2D diffuseTex : register (t2);
Texture2D SSAO_Tex	 : register (t3);
Texture2D ShadowMap  : register (t4);

SamplerState shadowMapSampler : register (s0);


float3 CalcLighting( float3 normal, float3 position, float3 diffuseAlbedo, float3 specularAlbedo,
	float specularPower, float3 lightPos, float3 lightDirection, float3 lightColor,	float3 ssao, float percentage);
float CalcShadowFactor(float2 uv);

cbuffer cb : register(b0)
{
	float4x4	view;
	float4x4	projection;
	float3		cameraPos;
};

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

	float4 lightPos = mul(t, mul(projection, mul(view, float4(position, 1.0f))));
	lightPos.xyz /= lightPos.w;


	float3 lighting = CalcLighting(normal, position, diffuseAlbedo, specularAlbedo, 
		specularPower,input.lightPos, input.lightDirection, input.lightColor, ssao, CalcShadowFactor(lightPos.xy));

	return float4( lighting, 1.0f );
}


//################################
//		HELPER FUNCTIONS
//################################
float3 CalcLighting(float3 normal, float3 position,	float3 diffuseAlbedo, float3 specularAlbedo,
	float specularPower, float3 lightPos, float3 lightDirection, float3 lightColor,	float3 ssao, float percentage)
{
	float attenuation = 1.0f;
	float3 L = -lightDirection;
	L = mul(view, float4(L, 0.f)).xyz;

	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * lightColor * diffuseAlbedo * pow(ssao, 10) * percentage;

	// Calculate the specular term
	float3 V = normalize(cameraPos - position);
	V = mul(view, float4(V, 0.f)).xyz;

	float3 H = normalize( L + V );
	float3 specular = pow( saturate( dot(normal, H) ), specularPower ) *
							 lightColor * specularAlbedo.xyz * nDotL;
	// Final value is the sum of the albedo and diffuse with attenuation applied

	return saturate(( diffuse + specular) * attenuation);
}


float CalcShadowFactor(float2 uv)
{
	const float dx = 1.f / 1080.f;
	const float dy = 1.f / 720.f;

	float percentLit = 0.0f;
	const float2 offsets[9] = 
	{
		float2(-dx,  -dy), float2(0.0f,  -dy), float2(dx,  -dy),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dy), float2(0.0f,  +dy), float2(dx,  +dy)
	};

	[unroll]
	for(int i = 0; i < 9; ++i)
	{
		percentLit += ShadowMap.Sample(shadowMapSampler, uv + offsets[i]).x;
		//percentLit += ShadowMap.Load(int3(uv + offsets[i],0)).x;
	}
	return percentLit /= 9.f;
}