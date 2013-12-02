#pragma pack_matrix(row_major)
SamplerState m_textureSampler	: register (s0);
Texture2D diffuse	: register(t0);
Texture2D normalMap	: register(t1);
Texture2D specular  : register(t2);

cbuffer cb : register(b1)
{
	float4x4 view;
	float4x4 projection;
	float3 cameraPos;
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
	float4	pos					: SV_POSITION;
	float4	wpos				: WSPOSITION;
	//float3	diffuse				: COLOR;
	//float	specularPower		: SP;
	//float	specularIntensity	: SI;
	//float3	normal				: NORMAL;
	float3 normal	: NORMAL;
	float2 uvCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
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

	output.pos = mul( projection, mul(view, input.pos) );
	output.wpos = input.pos;

	output.normal = normalize(input.normal);
	output.uvCoord = input.uvCoord;
	output.tangent = normalize(input.tangent);
	output.binormal = normalize(input.binormal);


	//output.diffuse = diffuse.Sample(m_textureSampler, input.uvCoord);//input.color.xyz;
	//output.specularPower = specular.Sample(m_textureSampler, input.uvCoord).x;//input.normal.w;
	//output.specularIntensity = 1.0f;
	//output.normal = normalMap.Sample(m_textureSampler, input.uvCoord).xyz;//input.normal.xyz;
	
	return output;
}

PSOut PS( PSIn input )
{
	PSOut output;
	//float3 norm				= 0.5f * (normalize(input.normal) + 1.0f);
	float4 bumpMap = normalMap.Sample(m_textureSampler, input.uvCoord);
	bumpMap = (bumpMap *2.0f) - 1.0f;
	float3 normal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
	normal = 0.5f * normalize(bumpMap.xyz) + 1.0f;
	

	output.diffuse.xyz		= diffuse.Sample(m_textureSampler, input.uvCoord).xyz;//input.diffuse.xyz;
	output.diffuse.w		= 1.0f; // Empty. 1.0f for debug.
	output.normal.w			= 1.0f;//specular.Sample(m_textureSampler, input.uvCoord).x;//input.specularPower;// 1.0f for debug.
	output.normal.xyz		= normal;//norm.xyz;
	output.wPosition.xyz	= float3(input.wpos.x, input.wpos.y, input.wpos.z);
	output.wPosition.w		= 1.0f;//input.specularIntensity; // 1.0f for debug.

	return output;
}