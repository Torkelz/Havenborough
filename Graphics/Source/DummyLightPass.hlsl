#pragma pack_matrix(row_major)

SamplerState m_textureSampler	: register (s0);
Texture2D depthTex				: register (t0);
Texture2D normalTex				: register (t1);
Texture2D diffuseTex			: register (t2);
Texture2D specularTex			: register (t3);

cbuffer cb : register(b0)
{
	float4x4	invview;
	float4x4	invprojection;
	float3		cameraPos;
};
cbuffer LightParams : register(b1)
{
    float3	lightPos;
    float3	lightColor;
	float3	lightDirection;
    float2	spotlightAngles;
    float	lightRange;
	int		lightType;
};

//################################
//		HELPER FUNCTIONS
//################################
void GetGBufferAttributes( in float2 screenPos, 
						  out float3 normal,
						  out float3 position,
						  out float3 diffuseAlbedo,
						  out float3 specularAlbedo,
						  out float specularPower)
{
	float3 normal2 = normalTex.Sample(m_textureSampler, screenPos).xyz;
	normal = (normal2 * 2) - 1.0f;

	position = depthTex.Sample(m_textureSampler, screenPos).xyz;

	diffuseAlbedo = diffuseTex.Sample(m_textureSampler, screenPos).xyz;
	float4 spec = float4(0,0,0,0);//specularTex.Sample(m_textureSampler, screenPos).xyz;//specularTex.Load( sampleIndices );

	specularAlbedo = spec.xyz;
	specularPower = spec.w;
}

float3 CalcLighting(	float3 normal,
						float3 position,
						float3 diffuseAlbedo,
						float3 specularAlbedo,
						float specularPower )
{
	float3 L = float3(0.f,0.f,0.f);
	float attenuation = 1.0f;
	if( lightType == 0 || lightType == 1) //Point light||Spot light
	{
		L = lightPos - position;
		float dist = length( L );
		attenuation = max( 0.f, 1.0f - (dist / lightRange) );
		L /= dist;
	}
	else if (lightType == 2) //Directional light
	{
		L = -lightDirection;
	}
	
	if (lightType == 1) //Spot light
	{
		float3 L2 = lightDirection;
		float rho = dot( -L, L2 );
		attenuation *= saturate( (rho - spotlightAngles.y) /
										(	spotlightAngles.x -
											spotlightAngles.y ) );
	}

	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse;
	if(lightType != 0)
		diffuse = nDotL * lightDirection * diffuseAlbedo;
	else
		diffuse = nDotL * diffuseAlbedo;

	// Calculate the specular term
	float3 V = cameraPos - position;
	float3 H = normalize( L + V );
	float3 specular = pow( saturate( dot(normal, H) ), specularPower ) *
							lightColor * specularAlbedo.xyz * nDotL;
	// Final value is the sum of the albedo and diffuse with attenuation applied
	return ( diffuse + specular ) * attenuation;
}


//##################################
//		SHADER PASSES
//##################################
struct VSOutput
{
	float4	position : SV_Position;
	float2  texCoord : TEXCOORD;
};

VSOutput VSmain( uint vID : SV_VERTEXID )
{
	VSOutput output;
	/*if(vID != 4)
		output.texCoord = float2( ( vID << 1 ) & 2, vID & 2 );
	else
		output.texCoord = float2(0.0f,2.0f);*/
	switch(vID)
	{
		case 0:		output.texCoord = float2(0,0);	break;
		case 1:		output.texCoord = float2(1,0);	break;
		case 2:		output.texCoord = float2(0,1);	break;
		case 3:		output.texCoord = float2(0,1);	break;
		case 4:		output.texCoord = float2(1,0);	break;
		case 5:		output.texCoord = float2(1,1);	break;
		default:	output.texCoord = float2(-1,-1);break;
	}
	output.position = float4( (output.texCoord * float2(2.0f,-2.0f)) + 
								float2(-1.0f,1.0f),
								0.0f,
								1.0f);
	return output; 
}

float4 PSmain( VSOutput input ) :  SV_Target
{
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;
	
	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes( input.texCoord, normal, position, diffuseAlbedo,
		specularAlbedo, specularPower );

	float3 lighting = CalcLighting( normal, position, diffuseAlbedo,
		specularAlbedo, specularPower );
	
	return float4( lighting, 1.0f );
}