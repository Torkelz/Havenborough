#pragma pack_matrix(row_major)
#include "LightHelper.hlsl"

Texture2D wPosTex	 : register (t0);
Texture2D normalTex	 : register (t1);
Texture2D diffuseTex : register (t2);
Texture2D SSAO_Tex	 : register (t3);

float3 CalcLighting( float3 normal, float3 position, float3 diffuseAlbedo, float3 specularAlbedo,
	float specularPower, float3 lightPos, float3 lightDirection, float3 lightColor,	float3 ssao);

cbuffer cb : register(b0)
{
	float4x4	view;
	float4x4	projection;
	float3		cameraPos;
	float4		ssaoScale;
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
	GetGBufferAttributes(input.vposition.xy, ssaoScale, normalTex, diffuseTex, SSAO_Tex, wPosTex,
		normal, diffuseAlbedo, specularAlbedo, ssao, position, specularPower);
	
	float3 lighting = CalcLighting(normal, position, diffuseAlbedo, specularAlbedo, 
		specularPower,input.lightPos, input.lightDirection, input.lightColor, ssao);

	return float4( lighting, 1.0f );
}


//################################
//		HELPER FUNCTIONS
//################################
float3 CalcLighting(float3 normal, float3 position,	float3 diffuseAlbedo, float3 specularAlbedo,
	float specularPower, float3 lightPos, float3 lightDirection, float3 lightColor,	float3 ssao)
{
	float attenuation = 1.0f;
	float3 L = -lightDirection;
	L = mul(view, float4(L, 0.f)).xyz;

	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * lightColor * diffuseAlbedo * pow(ssao, 10);

	// Calculate the specular term
	float3 V = normalize(cameraPos - position);
	V = mul(view, float4(V, 0.f)).xyz;

	float3 H = normalize( L + V );
	float3 specular = pow( saturate( dot(normal, H) ), specularPower ) *
							 lightColor * specularAlbedo.xyz * nDotL;
	// Final value is the sum of the albedo and diffuse with attenuation applied

	return saturate(( diffuse + specular) * attenuation );
}