#pragma pack_matrix(row_major)

Texture2D wPosTex					: register (t0);
Texture2D normalTex					: register (t1);
Texture2D diffuseTex				: register (t2);

float4x4 calcRotationMatrix(float3 direction, float3 position);

float3 CalcLighting(	float3 normal,
						float3 position,
						float3 diffuseAlbedo,
						float3 specularAlbedo,
						float specularPower,
						float3 lightPos,
						float lightRange,
						float3 lightDirection,
						float2 spotlightAngles,
						float3 lightColor);

void GetGBufferAttributes( in float2 screenPos, 
						  out float3 normal,
						  out float3 position,
						  out float3 diffuseAlbedo,
						  out float3 specularAlbedo,
						  out float specularPower);

cbuffer cb : register(b0)
{
	float4x4	view;
	float4x4	projection;
	float3		cameraPos;
	int			nrLights;
};
struct VSInput
{
	float3	vposition		: POSITION;
	float3	lightPos		: LPOSITION;
    float3	lightColor		: COLOR;
	float3	lightDirection	: DIRECTION;
    float2	spotlightAngles	: ANGLE;
    float	lightRange		: RANGE;
	uint		lightType	: TYPE;
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

//###########################
// Shader steps Vertex Shader
//############################
VSOutput SpotLightVS(VSInput input)
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

	VSOutput output;
	output.vposition		= mul(projection, mul(view, pos));
	output.lightPos			= input.lightPos;
	output.lightColor		= input.lightColor;	
	output.lightDirection	= direction;	
	output.spotlightAngles	= input.spotlightAngles;	
	output.lightRange		= input.lightRange;	
	return output;
}
//##lightType		#########################
// Shader steps Pixel Shader
//############################
float4 SpotLightPS( VSOutput input ) : SV_TARGET
{
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;
	
	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes( input.vposition.xy, normal, position, diffuseAlbedo,
		specularAlbedo, specularPower );

	float3 lighting = CalcLighting( normal, position, diffuseAlbedo,
							specularAlbedo, specularPower,input.lightPos,input.lightRange,
							input.lightDirection, input.spotlightAngles, input.lightColor);

	return float4( lighting, 1.0f );
}



void GetGBufferAttributes( in float2 screenPos, 
						  out float3 normal,
						  out float3 position,
						  out float3 diffuseAlbedo,
						  out float3 specularAlbedo,
						  out float specularPower)
{
	int3 sampleIndex = int3(screenPos,0);
	float4 normalTexSample = normalTex.Load(sampleIndex).xyzw;	

	float3 normal2 = normalTexSample.xyz;
	specularPower = normalTexSample.w;
	normal = (normal2 * 2.0f) - 1.0f;

	diffuseAlbedo = diffuseTex.Load(sampleIndex).xyz;	

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
						float3 lightDirection,
						float2 spotlightAngles,
						float3 lightColor)
{
	float3 L = lightPos - position;
	float dist = length( L );
	float attenuation = max( 0.f, 1.0f - (dist / lightRange) );
	L /= dist;
	
	float2 spotlightAngle = spotlightAngles;

	float3 L2 = lightDirection;
	float rho = dot( -L, L2 );
	attenuation *= saturate( (rho - spotlightAngle.y) /
							(spotlightAngle.x - spotlightAngle.y) );
	if(attenuation == 0.f)
		return float3(0,0,0);
	
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
	//return rotation;
}