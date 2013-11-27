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

struct PSOut
{
	half4 diffuse	: SV_Target0; // xyz = diffuse color, w = empty
	half4 normal	: SV_Target1; // xy = normal.xy, z = specularPower, w = specularIntensity
};

PSIn VS( VSIn input )
{
	PSIn output;

	output.pos = mul( mul(input.pos, view), projection );
	output.diffuse = input.color.xyz;
	output.specularPower = input.normal.w;
	output.specularIntensity = input.color.w;
	output.normal = input.normal.xyz;

	return output;
}

PSOut PS( PSIn input )
{
	PSOut output;
	float2 norm			= input.normal.xy;
	output.diffuse.xyz	= input.diffuse.xyz;
	output.diffuse.w	= 1.0f;
	output.normal.z		= input.specularPower;
	output.normal.w		= 1.0f;//input.specularIntensity; // 1.0f for debug.
	output.normal.xy	= 0.5f * (normalize(norm) + 1.0f);

	return output;
}