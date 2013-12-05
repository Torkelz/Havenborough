#pragma pack_matrix(row_major)

struct Lights
{
	float3	lightPos;
	int		padd1;
    float3	lightColor;
	int		padd2;
	float3	lightDirection;
	int		padd3;
    float2	spotlightAngles;
    float	lightRange;
	int		lightType;
};

SamplerState m_textureSampler		: register (s0);
Texture2D wPosTex					: register (t0);
Texture2D normalTex					: register (t1);
Texture2D diffuseTex				: register (t2);
StructuredBuffer<Lights> m_lights	: register (t3);	

cbuffer cb : register(b0)
{
	float4x4	view;
	float4x4	projection;
	float3		cameraPos;
	int			nrLights;
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
	float4 normalTexSample = normalTex.Sample(m_textureSampler, screenPos);	

	float3 normal2 = normalTexSample.xyz;
	specularPower = normalTexSample.w;
	normal = (normal2 * 2.0f) - 1.0f;

	diffuseAlbedo = diffuseTex.Sample(m_textureSampler, screenPos).xyz;

	float4 wPosTexSample = wPosTex.Sample(m_textureSampler, screenPos);
	position = wPosTexSample.xyz;
	specularAlbedo = wPosTexSample.w;
}

float3 CalcLighting(	float3 normal,
						float3 position,
						float3 diffuseAlbedo,
						float3 specularAlbedo,
						float specularPower,
						int i)
{
	int lightType = m_lights[i].lightType;
	float3 L = float3(0.f,0.f,0.f);
	float attenuation = 1.0f;

	if( lightType == 0 || lightType == 1) //Point light||Spot light
	{
		L = m_lights[i].lightPos - position;
		float dist = length( L );
		attenuation = max( 0.f, 1.0f - (dist / m_lights[i].lightRange) );
		L /= dist;
	}
	else if (lightType == 2) //Directional light
	{
		L = -m_lights[i].lightDirection;
	}
	
	if (lightType == 1) //Spot light
	{
		float2 spotlightAngle = m_lights[i].spotlightAngles;

		float3 L2 = m_lights[i].lightDirection;
		float rho = dot( -L, L2 );
		attenuation *= saturate( (rho - spotlightAngle.y) / (spotlightAngle.x - spotlightAngle.y) );
	} 

	float3 lightColor = m_lights[i].lightColor;

	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * lightColor * diffuseAlbedo;

	// Calculate the specular term
	float3 V = normalize(cameraPos - position);
	float3 H = normalize( L + V );
	float3 specular = pow( saturate( dot(normal, H) ), specularPower ) *
							 lightColor * specularAlbedo.xyz * nDotL;
	// Final value is the sum of the albedo and diffuse with attenuation applied
	return saturate(( diffuse + specular ) * attenuation);
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

	//REMOVE IF STRANGE RESULTS HAPPEN
	if(normal.x == 0 && normal.y == 0 && normal.z == 0)
		return float4(diffuseAlbedo,1);

	float3 lightPosition;
	float lightRange;
	
	float3 lighting = float3(0,0,0);

	bool calcLight = false;

	//World space scissor-test
	for(int i = 0; i < nrLights; i++)
	{
		lightPosition = m_lights[i].lightPos;
		lightRange = m_lights[i].lightRange;
		int lightType = m_lights[i].lightType;
		if( lightType == 0 || lightType == 1 )
				if(	position.x < (lightPosition.x + lightRange) && 
					position.x > (lightPosition.x - lightRange) &&
					position.y < (lightPosition.y + lightRange) && 
					position.y > (lightPosition.y - lightRange) &&
					position.z < (lightPosition.z + lightRange) && 
					position.z > (lightPosition.z -	lightRange))
						calcLight = true;

		if(calcLight)
			lighting += CalcLighting( normal, position, diffuseAlbedo,
									specularAlbedo, specularPower,i );
		calcLight = false;
	}

	return float4( lighting, 1.0f );
}