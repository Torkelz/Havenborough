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
						float3 lightDirection,
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

VSOutput DirectionalLightVS(VSInput input)
{
	VSOutput output;
	output.vposition		= float4(input.vposition,1.0f);
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
float4 DirectionalLightPS( VSOutput input ) : SV_TARGET
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
							specularAlbedo, specularPower,input.lightPos,
							input.lightDirection, input.lightColor, ssao);

	return float4( lighting, 1.0f );
}


//################################
//		HELPER FUNCTIONS
//################################
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
						float3 lightDirection,
						float3 lightColor,
						float3 ssao)
{
	float attenuation = 1.0f;
	float3 L = -lightDirection;
	L = mul(view, float4(L, 0.f)).xyz;

	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * lightColor * diffuseAlbedo;

	// Calculate the specular term
	float3 V = normalize(cameraPos - position);
	V = mul(view, float4(V, 0.f)).xyz;

	float3 H = normalize( L + V );
	float3 specular = pow( saturate( dot(normal, H) ), specularPower ) *
							 lightColor * specularAlbedo.xyz * nDotL;
	// Final value is the sum of the albedo and diffuse with attenuation applied

	return saturate(( diffuse + specular ) * attenuation * ssao);
}