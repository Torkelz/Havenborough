#pragma pack_matrix(row_major)

cbuffer cb : register(b0)
{
	float4x4 cView;
	float4x4 cProjection;
	float3	 cCameraPos;
	float	cSSAOScale;
};

cbuffer cbWorld : register(b1)
{
	float4x4 cWorld;
};

struct VSIn
{
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 uvCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
	float4x4 vworld	: WORLD;
};

float4 VS(VSIn input) : SV_POSITION
{
	float4 output;
	output = mul(cProjection, mul(cView, mul(input.vworld, input.pos)));

	return output;
}

float4 PS(float4 position : SV_POSITION) : SV_TARGET
{
	return position;
}