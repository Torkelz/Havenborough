#pragma pack_matrix(row_major)

cbuffer cb : register(b1)
{
	float4x4 view;
	float4x4 projection;
	float3	 cameraPos;
	float		cSSAOScale;
};

//##################################
//		SHADER PASSES				##
//##################################
struct VSOutput
{
	float4 position : SV_Position;
};

VSOutput VS( float4 vertexPosition : POSITION )
{
	VSOutput output;
	output.position = mul( projection, mul(view, vertexPosition) );
	return output; 
}

float4 PS( VSOutput input ) :  SV_Target0
{
	return float4(0.2f, 1.f, 0.2f, 1.f);
}