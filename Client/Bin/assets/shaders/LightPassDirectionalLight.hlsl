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

float3 CalcLighting( float3 normal, float3 position, float3 diffuseAlbedo, float3 specularAlbedo,
	float specularPower, float3 lightPos, float3 lightDirection, float3 lightColor, float3 ssao, float4 light);
float CalcShadowFactor(float3 uv, float nDotL);
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
	float innerBorder = SHADOW_BORDER;
	float outerBorder = 1 - SHADOW_BORDER;
	if(shadowMapped == 0)
	{
		lighting = CalcLighting(normal, position, diffuseAlbedo, specularAlbedo, 
				specularPower,input.lightPos, input.lightDirection, input.lightColor, ssao, lightPos);
	}
	else if(big == 0)
	{
		if(lightPos.x > 0.f && lightPos.x < 1.f && lightPos.y > 0.f && lightPos.y < 1.f)
		{
			lighting = CalcLighting(normal, position, diffuseAlbedo, specularAlbedo, 
				specularPower,input.lightPos, input.lightDirection, input.lightColor, ssao, lightPos);
		}
	}
	else
	{
		if(lightPos.x < innerBorder || lightPos.x > outerBorder || lightPos.y < innerBorder || lightPos.y > outerBorder)
		{
			lighting = CalcLighting(normal, position, diffuseAlbedo, specularAlbedo, 
				specularPower,input.lightPos, input.lightDirection, input.lightColor, ssao, lightPos);
		}
	}
	return float4( lighting * input.lightIntensity, 1.0f );
}


//################################
//		HELPER FUNCTIONS
//################################
float3 CalcLighting(float3 normal, float3 position,	float3 diffuseAlbedo, float3 specularAlbedo,
	float specularPower, float3 lightPos, float3 lightDirection, float3 lightColor, float3 ssao, float4 light)
{

	float attenuation = 1.0f;
	float3 L = -lightDirection;
	L = mul(view, float4(L, 0.f)).xyz;

	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * lightColor * diffuseAlbedo * pow(ssao, 10);
	
	diffuse *=  CalcShadowFactor(light.xyz, nDotL);

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



float CalcShadowFactor(float3 uv, float nDotL)
{
	float percentLit = 1.0f;
	 if ((saturate(uv.x) == uv.x) &&
        (saturate(uv.y) == uv.y) && uv.z > 0)
    {
        // Use an offset value to mitigate shadow artifacts due to imprecise 
        // floating-point values (shadow acne).
        //
        // This is an approximation of epsilon * tan(acos(saturate(NdotL))):
        float margin = acos(nDotL);

        // The offset can be slightly smaller with smoother shadow edges.
        float epsilon = 0.0005 / margin;

        //float epsilon = 0.001 / margin;

        // Clamp epsilon to a fixed range so it doesn't go overboard.
        epsilon = clamp(epsilon, 0, 0.1);

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
			value += ShadowMap.SampleCmpLevelZero(shadowMapSampler, float2(uv.x + (i - 10) * SMAP_DX, uv.y + (i - 10) * SMAP_DX), uv.z) * coefficients[i];
			//value += ShadowMap.SampleCmpLevelZero(shadowMapSampler, float2(uv.x, uv.y + (i - 10) * SMAP_DX), uv.z) * coefficients[i];
		}
		percentLit = value;// * 0.5f;
	 }
	return percentLit;

}