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
};

cbuffer cbWorld : register(b1)
{
	float4x4 cWorld;
};

cbuffer cbSkinned : register(b2)
{
	//Lower later if we realize we need fewer than 96 bones per character.
	float4x4 cWorldInvTranspose;
	float4x4 cBoneTransform[96];
};

struct VSIn
{
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 uvCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
	float3 weights	: WEIGHTS;
	uint4 boneId	: BONEID;
};

//############################
// Shader step: Vertex Shader
//############################
PSIn VS(VSIn input)
{
	PSIn output;

	// Init array or else we get strange warnings about SV_POSITION.
	float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	weights[0] = input.weights.x;
	weights[1] = input.weights.y;
	weights[2] = input.weights.z;
	weights[3] = 1.f - input.weights.x - input.weights.y - input.weights.z;

	float4 posL			= float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 normalL		= float3(0.0f, 0.0f, 0.0f);
	float3 tangentL		= float3(0.0f, 0.0f, 0.0f);
	float3 binormalL	= float3(0.0f, 0.0f, 0.0f);

	[unroll]
	for(int i = 0; i < 4; ++i)
	{
	    // Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.
	    posL		+= weights[i] * mul(cBoneTransform[input.boneId[i] - 1], input.pos);
		normalL		+= weights[i] * mul((float3x3)cBoneTransform[input.boneId[i] - 1], input.normal);
		tangentL	+= weights[i] * mul((float3x3)cBoneTransform[input.boneId[i] - 1], input.tangent);
		binormalL	+= weights[i] * mul((float3x3)cBoneTransform[input.boneId[i] - 1], input.binormal);
	}
	posL /= posL.w;

	output.position = mul( cProjection, mul(cView, mul(cWorld, posL) ) );
	output.wposition = mul(cWorld, posL);

	output.normal = normalize(mul(cWorldInvTranspose, float4(normalL, 0.f))).xyz;
	output.uvCoord = input.uvCoord;
	output.tangent = normalize(mul(cWorld, float4(tangentL, 0.f))).xyz;
	output.binormal = normalize(mul(cWorld, float4(binormalL, 0.f))).xyz;
	output.depth = mul(cView, mul(cWorld, input.pos)).z;

	return output;
}

//############################
// Shader step: Pixel Shader
//############################
PSOut PS( PSIn input )
{
	return PSFunction(input, cView, diffuseTex, normalTex, specularTex, textureSampler);
}