

Texture2D depthTex		: register (t0);
Texture2D normalTex		: register (t1);
Texture2D diffuseTex	: register (t2);
Texture2D specularTex	: register (t3);

cbuffer cb : register(b0)
{
	float4x4 invview;
	float4x4 invprojection;
	float3 cameraPos;
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
						  out float specularPower,
						  out float depth)
{
	int3 sampleIndices = int3( screenPos, 0);
	normal = normalTex.Load( sampleIndices ).xyz;

	//transform 2d coord to 3D coord
	depth = depthTex.Load(sampleIndices).x;
	float4 pixelpos = float4( (screenPos * float2(2.0f,-2.0f)) + 
								float2(-1.0f,1.0f),
								0.0f,
								1.0f);
	position = float3(pixelpos.xy, depth);
	position = mul(position, mul(invview,invprojection));

	diffuseAlbedo = diffuseTex.Load( sampleIndices ).xyz;
	float4 spec = float4(0,0,0,0);//specularTex.Load( sampleIndices );

	specularAlbedo = spec.xyz;
	specularPower = spec.w;
}

float3 CalcLighting(	float3 normal,
						float3 position,
						float3 diffuseAlbedo,
						float3 specularAlbedo,
						float specularPower )
{
	float3 L = 0;
	float attenuation = 1.0f;
	if( lightType == 0 || lightType == 1) //Point light||Spot light
	{
		L = lightPos - position;
		float dist = length( L );
		attenuation = max( 0, 1.0f - (dist / lightRange) );
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
	float3 diffuse = nDotL * lightDirection * diffuseAlbedo;
	
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

float4 PSmain( VSOutput input ) :  SV_Target0
{
	float2 screenPos = input.texCoord;

	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;
	float depth;
	
	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes( screenPos, normal, position, diffuseAlbedo,
		specularAlbedo, specularPower, depth );

	return float4(depth,depth,depth,1);

	float3 lighting = CalcLighting( normal, position, diffuseAlbedo,
		specularAlbedo, specularPower );
	//return float4(1,0,0,1);
	return float4( lighting, 1.0f );
}