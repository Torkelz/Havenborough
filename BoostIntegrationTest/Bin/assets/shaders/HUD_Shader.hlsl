#pragma pack_matrix(row_major)
SamplerState cTextureSampler : register(s0);
Texture2D cHUD_Texture : register(t0);

cbuffer cb : register (b0)
{
	float4x4 cView;
	float4x4 cOrthoProjection;
};

cbuffer cbPos : register (b1)
{
	float4x4 cWorld;
};

struct VS_Input
{
	float4 position	: POSITION;
	float3 normal	: NORMAL;
	float2 texCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
};

struct VS_Output
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

VS_Output VS(VS_Input input)
{
	VS_Output output;
	output.position = mul(cOrthoProjection, mul(cWorld, float4(input.position.xyz, 1.0f)));
	output.texCoord = input.texCoord;
	
	return output;
}

float4 PS(VS_Output input) : SV_Target
{
	return cHUD_Texture.Sample(cTextureSampler, input.texCoord);
}