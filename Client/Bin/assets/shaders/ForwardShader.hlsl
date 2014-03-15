#pragma pack_matrix(row_major)

SamplerState m_textureSampler	: register(s0);
Texture2D diffuse				: register(t0);
Texture2D normalMap				: register(t1);

cbuffer cb : register(b1)
{
	float4x4 view;
	float4x4 projection;
	float3	 cameraPos;
	float		cSSAOScale;
};

cbuffer cbWorld : register(b2)
{
	float4x4 world;
};

cbuffer cbColor : register(b3)
{
	float3 cbColor;
};

struct VSIn
{
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 uvCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
};

struct PSIn
{
	float4 pos		: SV_POSITION;
	float4 wpos		: WSPOSITION;
	float3 normal	: NORMAL;
	float2 uvCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
};

struct PSOut
{
	float4 color	: SV_Target0;
};

PSIn VS( VSIn input )
{
	PSIn output;

	output.pos = mul( projection, mul(view, mul(world, input.pos) ) );
	output.wpos = mul(world, input.pos);

	output.normal = normalize(mul(world, float4(input.normal, 0.f)).xyz);
	output.uvCoord = input.uvCoord;
	output.tangent = normalize(mul(world, float4(input.tangent,0.f)).xyz);
	output.binormal = normalize(mul(world, float4(input.binormal, 0.f)).xyz);
	
	return output;
}

PSOut PS( PSIn input )
{
	PSOut output;

	output.color = diffuse.Sample(m_textureSampler, input.uvCoord);
	if(output.color.w == 0.f)
		discard;

	output.color.xyz = saturate(output.color.xyz * cbColor);
	return output;
}