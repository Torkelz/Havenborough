cbuffer cb : register(b1)
{
	float4x4 view;
	float4x4 projection;
	float3 cameraPos;
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
	float4	wpos				: WSPOSITION;
	float3	diffuse				: COLOR;
	float	specularPower		: SP;
	float	specularIntensity	: SI;
	float3	normal				: NORMAL;
};

struct PSOut
{
	half4 diffuse	: SV_Target0; // xyz = diffuse color, w = empty
	half4 normal	: SV_Target1; // xyz = normal.xyz, w = specularPower
	half4 wPosition	: SV_Target2; // xyz = world position, w = specular intensity
};

PSIn VS( VSIn input )
{
	PSIn output;

	output.pos = mul( mul(input.pos, view), projection );
	output.wpos = input.pos;

	output.diffuse = input.color.xyz;
	output.specularPower = input.normal.w;
	output.specularIntensity = input.color.w;
	output.normal = input.normal.xyz;

	return output;
}

PSOut PS( PSIn input )
{
	PSOut output;
	float3 norm				= 0.5f * (normalize(input.normal) + 1.0f);
	output.diffuse.xyz		= input.diffuse.xyz;
	output.diffuse.w		= 1.0f; // Empty. 1.0f for debug.
	output.normal.w			= 1.0f; // input.specularPower; 1.0f for debug.
	output.normal.xyz		= norm.xyz;
	output.wPosition.xyz	= float3(input.wpos.x, input.wpos.y, input.wpos.z);
	output.wPosition.w		= 1.0f;//input.specularIntensity; // 1.0f for debug.

	return output;
}