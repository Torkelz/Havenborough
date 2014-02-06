#pragma pack_matrix(row_major)

Texture2D wPosTex	 : register (t0);
Texture2D normalTex	 : register (t1);
Texture2D diffuseTex : register (t2);
Texture2D SSAO_Tex	 : register (t3);

float3 CalcLighting(	float3 normal,
						float3 position,
						float3 diffuseAlbedo,
						float3 specularAlbedo,
						float specularPower,
						float3 lightPos,
						float lightRange,
						float3 lightColor,
						float3 ssao);

void GetGBufferAttributes( in float2 screenPos, 
						  out float3 normal,
						  out float3 position,
						  out float3 diffuseAlbedo,
						  out float3 specularAlbedo,
						  out float specularPower,
						  out float3 ssao);

cbuffer cb : register(b0)
{
	float4x4	view;
	float4x4	projection;
	float3		cameraPos;
};
struct VSInput
{
	float3	vposition		: POSITION;
	float3	lightPos		: LPOSITION;
    float3	lightColor		: COLOR;
	float3	lightDirection	: DIRECTION;
    float2	spotlightAngles	: ANGLE;
    float	lightRange		: RANGE;
};

struct VSOutput
{
	float4	vposition		: SV_Position;
	float3	lightPos		: LPOSITION;
    float3	lightColor		: COLOR;
	float3	lightDirection	: DIRECTION;
    float2	spotlightAngles	: ANGLE;
    float	lightRange		: RANGE;
};

//############################
// Shader steps Vertex Shader
//############################
VSOutput PointLightVS(VSInput input)
{
	float s = input.lightRange;
	float3 t = input.lightPos;
	float4x4 scale = {  float4(s,0,0,0),
						float4(0,s,0,0),
						float4(0,0,s,0),
						float4(0,0,0,1)};
	float4x4 trans = {  float4(1,0,0,t.x),
						float4(0,1,0,t.y),
						float4(0,0,1,t.z),
						float4(0,0,0,1)};

	float4 pos = mul(scale, float4(input.vposition,1.0f));
	pos = mul(trans, pos);
	VSOutput output;
	output.vposition		= mul(projection, mul(view, pos));
	//output.vposition		= mul(projection, mul(view, float4(input.vposition,1)));
	output.lightPos			= input.lightPos;
	output.lightColor		= input.lightColor;	
	output.lightDirection	= input.lightDirection;	
	output.spotlightAngles	= input.spotlightAngles;	
	output.lightRange		= input.lightRange;
	return output;
}
//############################
// Shader steps Pixel Shader
//############################
float4 PointLightPS( VSOutput input ) : SV_TARGET
{
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;
	float3 ssao;
	
	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes( input.vposition.xy, normal, position, diffuseAlbedo,
		specularAlbedo, specularPower, ssao);

	float3 lighting = CalcLighting( normal, position, diffuseAlbedo,
							specularAlbedo, specularPower,input.lightPos,input.lightRange,
							input.lightColor, ssao);

	return float4( lighting, 1.0f );
	//return float4( float3(0,1,0), 1.0f );
}



void GetGBufferAttributes( in float2 screenPos, 
						  out float3 normal,
						  out float3 position,
						  out float3 diffuseAlbedo,
						  out float3 specularAlbedo,
						  out float specularPower,
						  out float3 ssao)
{
	int3 sampleIndex = int3(screenPos,0);
	float4 normalTexSample = normalTex.Load(sampleIndex).xyzw;	

	float3 normal2 = normalTexSample.xyz;
	normal = (normal2 * 2.0f) - 1.0f;
	
	specularPower = diffuseTex.Load(sampleIndex).w;
	
	diffuseAlbedo = diffuseTex.Load(sampleIndex).xyz;	
	
	ssao = SSAO_Tex.Load(sampleIndex).xyz;

	float4 wPosTexSample = wPosTex.Load(sampleIndex).xyzw;	
	position = wPosTexSample.xyz;
	specularAlbedo = wPosTexSample.w;
}

float3 CalcLighting(	float3 normal,
						float3 position,
						float3 diffuseAlbedo,
						float3 specularAlbedo,
						float specularPower,
						float3 lightPos,
						float lightRange,
						float3 lightColor,
						float3 ssao)
{
	float3 L = lightPos - position;
	float dist = length( L );
	float attenuation = max( 0.f, 1.f - (dist / lightRange) );
	L /= dist;
	L = mul(view, float4(L, 0.0f)).xyz;

	if(attenuation == 0.f)
		return float3(0,0,0);
	
	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * lightColor * diffuseAlbedo * pow(ssao, 10);

	// Calculate the specular term
	float3 V = normalize(cameraPos - position);
	V = mul(view, float4(V, 0.0f)).xyz;

	float3 H = normalize( L + V );
	float3 specular = pow( saturate( dot(normal, H) ), specularPower ) *
							 lightColor * specularAlbedo.xyz * nDotL;
	// Final value is the sum of the albedo and diffuse with attenuation applied
	return saturate(( diffuse + specular ) * attenuation);
}