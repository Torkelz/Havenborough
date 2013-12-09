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
						int lightType,
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
	float		lightType	: TYPE;
};

struct VSOutput
{
	float4	vposition		: SV_Position;
	float3	lightPos		: LPOSITION;
    float3	lightColor		: COLOR;
	float3	lightDirection	: DIRECTION;
    float2	spotlightAngles	: ANGLE;
    float	lightRange		: RANGE;
	float		lightType	: TYPE;
};

//###########################
// Shader steps Vertex Shader
//############################
VSOutput PointLightVS(VSInput input)
{
	float s = 50.0f;//input.spotlightAngles.x;
	float  l = input.lightRange;
	float3 t = input.lightPos;
	float4x4 scale = {  float4(s,0,0,0),
						float4(0,50,0,0),
						float4(0,0,s,0),
						float4(0,0,0,1)};
	float4x4 rotat = calcRotationMatrix(input.lightDirection, input.lightPos);
	float4x4 trans = {  float4(1,0,0,t.x),
						float4(0,1,0,t.y),
						float4(0,0,1,t.z),
						float4(0,0,0,1)};

	float4 pos = float4(input.vposition,1.0f);
	pos = mul(rotat, pos);
	pos = mul(scale, pos);
	//pos = mul(trans, pos);
	VSOutput output;
	output.vposition		= mul(projection, mul(view, pos));
	//output.vposition		= mul(projection, mul(view, float4(input.vposition,1)));
	output.lightPos			= input.lightPos;
	output.lightColor		= input.lightColor;	
	output.lightDirection	= input.lightDirection;	
	output.spotlightAngles	= input.spotlightAngles;	
	output.lightRange		= input.lightRange;	
	output.lightType		= input.lightType;	
	return output;
}
//##lightType		#########################
// Shader steps Pixel Shader
//############################
float4 PointLightPS( VSOutput input ) : SV_TARGET
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
							specularAlbedo, specularPower,0,input.lightPos,input.lightRange,
							input.lightDirection, input.spotlightAngles, input.lightColor);

	return float4( lighting, 1.0f );
	//return float4( float3(0,1,0), 1.0f );
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
						int lightType,
						float3 lightPos,
						float lightRange,
						float3 lightDirection,
						float2 spotlightAngles,
						float3 lightColor)
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
		float2 spotlightAngle = spotlightAngles;

		float3 L2 = lightDirection;
		float rho = dot( -L, L2 );
		attenuation *= saturate( (rho - spotlightAngle.y) / (spotlightAngle.x - spotlightAngle.y) );
	} 
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
	float4x4 ret;

	/*float3 SpotUp = float3(0,1,0);
	float3 target = position + direction;
	float3 zaxis = normalize(target - position);
	float3 xaxis = normalize(cross(SpotUp, zaxis));
	float3 yaxis = cross(zaxis, xaxis);

	ret[0][0] = xaxis.x; ret[0][1] = xaxis.x; ret[0][2] = xaxis.z;
	ret[1][0] = yaxis.x; ret[1][1] = yaxis.x; ret[1][2] = yaxis.z;
	ret[2][0] = zaxis.x; ret[2][1] = zaxis.x; ret[2][2] = zaxis.z;
	ret[3][0] = ret[3][1] = ret[3][2] = 0.f;

	ret[0][3] = -dot(xaxis, position);
	ret[1][3] = -dot(yaxis, position);
	ret[2][3] = -dot(zaxis, position);
	ret[3][3] = 1.f;*/

	float x = direction.x;
	float y = direction.y;
	float z = direction.z;

	float rotx = atan2( y, z );
	float roty = atan2( x * cos(rotx), z );
	float rotz = atan2( cos(rotx), sin(rotx) * sin(roty) );

	 if (z >= 0) {
		roty = -atan2( x * cos(rotx), z );
	 }else{
		roty = atan2( x * cos(rotx), -z );
	 }

	float4x4 rotX = {float4(1,0,0,0),
					float4(0,cos(rotx),-sin(rotx),0),
					float4(0,sin(rotx),cos(rotx),0),
					float4(0,0,0,1)};
	float4x4 rotY = {float4(cos(roty),0,sin(roty),0),
					float4(0,1,0,0),
					float4(-sin(roty),0,cos(roty),0),
					float4(0,0,0,1)};
	float4x4 rotZ = {float4(cos(rotz), -sin(rotz),0,0),
					float4(sin(rotz),cos(rotz),0,0),
					float4(0,0,1,0),
					float4(0,0,0,1)};
	ret = rotX * rotY * rotZ;

	return ret;
}