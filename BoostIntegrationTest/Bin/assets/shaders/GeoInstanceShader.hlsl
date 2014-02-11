#pragma pack_matrix(row_major)

SamplerState m_textureSampler	: register(s0);
Texture2D diffuse				: register(t0);
Texture2D normalMap				: register(t1);
Texture2D specular				: register(t2);

cbuffer cb : register(b0)
{
	float4x4 view;
	float4x4 projection;
	float3	 cameraPos;
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

struct PSIn
{
	float4	pos		: SV_POSITION;
	float4	wpos	: WSPOSITION;
	float3 normal	: NORMAL;
	float2 uvCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
	float depth		: DEPTH;
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

	output.pos = mul( projection, mul(view, mul(input.vworld, input.pos) ) );
	output.wpos = mul(input.vworld, input.pos);

	output.normal = normalize(mul(input.vworld, float4(input.normal, 0.f)).xyz);
	output.uvCoord = input.uvCoord;
	output.tangent = normalize(mul(input.vworld, float4(input.tangent, 0.f)).xyz);
	output.binormal = normalize(mul(input.vworld, float4(input.binormal, 0.f)).xyz);
	output.depth = mul(view, mul(input.vworld, input.pos)).z;
			
	return output;
}

PSOut PS( PSIn input )
{
	PSOut output;
	float3 norm		= 0.5f * (input.normal + 1.0f);
	float4 bumpMap	= normalMap.Sample(m_textureSampler, input.uvCoord);
	bumpMap			= (bumpMap * 2.0f) - 1.0f;
	float3 normal	= input.normal + bumpMap.x * input.tangent + -bumpMap.y * input.binormal;
	normal			= mul(view, float4(normal, 0.f)).xyz;
	normal			= 0.5f * (normalize(normal) + 1.0f);
	
	float4 diffuseColor = diffuse.Sample(m_textureSampler, input.uvCoord);

	if(diffuseColor.w >= 0.7f)
		diffuseColor.w = 1.0f;
	else
		diffuseColor.w = 0.0f;

	if(diffuseColor.w == 1.0f)
	{
		output.diffuse			= float4(diffuseColor.xyz,1.0f);//input.diffuse.xyz;
		output.normal.w			= input.depth;
		output.normal.xyz		= normal;//normalize(mul((float3x3)view, normal));
		output.wPosition.xyz	= float3(input.wpos.x, input.wpos.y, input.wpos.z);
		output.wPosition.w		= specular.Sample(m_textureSampler, input.uvCoord).x;
	}
	else // If alpha is 0. Do not blend with any previous render targets.
	{
		discard;
	}

	return output;
}