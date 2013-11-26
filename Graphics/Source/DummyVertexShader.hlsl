cbuffer cb
{
	float4x4 view;
	float4x4 projection;
};

struct VSIn
{
	float4 pos		: POSITION;
	float4 normal	: NORMAL;
	float4 color	: COLOR;
};

struct PSIn
{
	float4	pos					: SV_POSITION;
	float3	diffuse				: COLOR;
	float	specularPower		: SP;
	float	specularIntensity	: SI;
	float3	normal				: NORMAL;
};

//struct PSOut
//{
//	float4 diffuse	: SV_Target0; // xyz = diffuse color, w = specularPower
//	float4 normal	: SV_Target1; // xyz = normal.xyz, w = specularIntensity
//	float4 depth	: SV_Target2; // Fetefete depth until further notice.
//	float4 depth2	: SV_Target3; // Fetefete depth until further notice.
//};

PSIn VSmain( VSIn input )
{
	PSIn output;

	output.pos = mul( mul(input.pos, view), projection );
	output.diffuse = input.color.xyz;
	output.specularPower = input.normal.w;
	output.specularIntensity = input.color.w;
	output.normal = input.normal.xyz;

	return output;
}

float4 PSmain( PSIn input ) : SV_Target
{
	/*PSOut output;

	output.diffuse.xyz	= input.diffuse.xyz;
	output.diffuse.w	= input.specularPower;
	output.normal.w		= input.specularIntensity;
	output.normal.xyz	= 0.5f * (normalize(input.normal) + 1.0f);
	output.depth		= input.pos.z / input.pos.w;
	output.depth2		= input.pos.z / input.pos.w;

	return output;*/

	return float4(1,0,0,1);
}