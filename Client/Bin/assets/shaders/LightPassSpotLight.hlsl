#pragma pack_matrix(row_major)
#include "DeferredHelperFunctions.hlsl"
#include "LightHelperFunctions.hlsl"

Texture2D wPosTex	 : register (t0);
Texture2D normalTex	 : register (t1);
Texture2D diffuseTex : register (t2);
Texture2D SSAO_Tex	 : register (t3);

float4x4 calcRotationMatrix(float3 direction, float3 position);

float3 CalcLighting(float3 normal, float3 position,	float3 diffuseAlbedo, float3 specularAlbedo,
	float specularPower, float3 lightPos, float lightRange,	float3 lightDirection, 
	float2 spotlightAngles,	float3 lightColor, float3 ssao);

cbuffer cb : register(b0)
{
	float4x4	view;
	float4x4	projection;
	float3		cameraPos;
	int			nrLights;
};

//###########################
// Shader step: Vertex Shader
//############################
VSLightOutput SpotLightVS(VSLightInput input)
{	
	float  l = input.lightRange;
	float s = (l*tan(acos(input.spotlightAngles.y)));//sqrt(0.5618f);

	float3 t = input.lightPos;
	float4x4 scale = {  float4(s,0,0,0),
						float4(0,s,0,0),
						float4(0,0,l,0),
						float4(0,0,0,1)};
	float4x4 rotat = calcRotationMatrix(input.lightDirection, input.lightPos);
	float4x4 trans = {  float4(1,0,0,t.x),
						float4(0,1,0,t.y),
						float4(0,0,1,t.z),
						float4(0,0,0,1)};

	float4 pos = float4(input.vposition,1.0f);
	pos = mul(scale, pos);
	pos = mul(rotat, pos);	
	pos = mul(trans, pos);

	float3 direction = input.lightDirection;
	if((direction.x == 0.f && direction.z == 0.f ) && (direction.y == 1.f || direction.y == -1.f))
	{
		float neg = 1.f;
		if(direction.y < 0)
			neg = -1.f;
		direction.x = 0.001f * neg;
		direction.z = 0.001f * neg;
		direction.y = 0.998f * neg;
		direction = normalize(direction);
	}

	VSLightOutput output;
	output.vposition		= mul(projection, mul(view, pos));
	output.lightPos			= input.lightPos;
	output.lightColor		= input.lightColor;	
	output.lightDirection	= direction;	
	output.spotlightAngles	= input.spotlightAngles;	
	output.lightRange		= input.lightRange;	
	return output;
}
//###########################
// Shader step: Pixel Shader
//############################
float4 SpotLightPS(VSLightOutput input) : SV_TARGET
{
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;
	float3 ssao;
	
	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes(input.vposition.xy, normalTex, diffuseTex, SSAO_Tex, wPosTex,
		normal, diffuseAlbedo, specularAlbedo, ssao, position, specularPower);

	float3 lighting = CalcLighting(normal, position, diffuseAlbedo,	specularAlbedo, 
		specularPower,input.lightPos,input.lightRange, input.lightDirection,
		input.spotlightAngles, input.lightColor, ssao);

	return float4( lighting, 1.0f );
}

float3 CalcLighting(float3 normal, float3 position,	float3 diffuseAlbedo, float3 specularAlbedo,
	float specularPower, float3 lightPos, float lightRange,	float3 lightDirection,
	float2 spotlightAngles,	float3 lightColor, float3 ssao)
{
	float3 L = lightPos - position;
	float dist = length( L );
	float attenuation = max( 0.f, 1.0f - (dist / lightRange) );
	L /= dist;
	L = mul(view, float4(L, 0.0f)).xyz;
	

	float2 spotlightAngle = spotlightAngles;

	float3 L2 = lightDirection;
	L2 = mul(view, float4(L2, 0.0f)).xyz;

	float rho = dot( -L, L2 );
	attenuation *= saturate( (rho - spotlightAngle.y) /
							(spotlightAngle.x - spotlightAngle.y) );
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

float4x4 calcRotationMatrix(float3 direction, float3 position)
{
	float3 fwd = direction;
	float3 up = float3(0,1,0);
	float3 side = normalize(cross(up,fwd));
	up = normalize(cross(side,fwd));

	float4x4 rotation = {float4(side.x,side.y,side.z,0),
						float4(up.x, up.y,up.z,0),
						float4(fwd.x,fwd.y,fwd.z,0),
						float4(0,0,0,1)
						};

	return transpose(rotation);
}