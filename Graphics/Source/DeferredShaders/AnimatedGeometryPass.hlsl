#pragma pack_matrix(row_major)

SamplerState m_textureSampler	: register(s0);
Texture2D diffuse				: register(t0);
Texture2D normalMap				: register(t1);
Texture2D specular				: register(t2);

cbuffer cb : register(b1)
{
	float4x4 view;
	float4x4 projection;
	float3	 cameraPos;
	int		 ninjaKick;
};

cbuffer cbWorld : register(b2)
{
	float4x4 world;
	float4x4 worldInvTranspose;
};

cbuffer cbSkinned : register(b3)
{
	//Lower later if we realize we need fewer than 96 bones per character.
	float4x4 boneTransform[96];
};

struct VSIn
{
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 uvCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
	float3 weights	: WEIGHTS;
	uint4 boneId	: BONEID;
};

struct PSIn
{
	float4	pos		: SV_POSITION;
	float4	wpos	: WSPOSITION;
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

PSIn VS(VSIn input)
{
    PSIn output;

	// Init array or else we get strange warnings about SV_POSITION.
	float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	weights[0] = input.weights.x;
	weights[1] = input.weights.y;
	weights[2] = input.weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL			= float3(0.0f, 0.0f, 0.0f);
	float3 normalL		= float3(0.0f, 0.0f, 0.0f);
	float3 tangentL		= float3(0.0f, 0.0f, 0.0f);
	float3 binormalL	= float3(0.0f, 0.0f, 0.0f);
	for(int i = 0; i < 4; ++i)
	{
	    // Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.
	    posL		+= weights[i] * mul(input.pos, boneTransform[input.boneId[i]]).xyz;
		normalL		+= weights[i] * mul(input.normal,  (float3x3)boneTransform[input.boneId[i]]);
		tangentL	+= weights[i] * mul(input.tangent, (float3x3)boneTransform[input.boneId[i]]);
		binormalL	+= weights[i] * mul(input.binormal, (float3x3)boneTransform[input.boneId[i]]);
	}
 
	// Transform to view space.
	//vout.PosV    = mul(float4(posL, 1.0f), gWorldView).xyz;
	//vout.NormalV = mul(normalL, (float3x3)gWorldInvTransposeView);
		
	// Transform to homogeneous clip space.
	//vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	//vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	//output.pos = mul( projection, mul(view, mul(world, input.pos) ) );
	//output.wpos = mul(world, input.pos);

	output.pos = mul( projection, mul(view, mul(world, float4(posL, 1.0f)) ) );
	output.wpos = mul(world, float4(posL, 1.0f));

	output.normal = normalize(mul(worldInvTranspose, normalL));
	output.uvCoord = input.uvCoord;
	output.tangent = normalize(mul(world, tangentL));
	output.binormal = normalize(mul(world, binormalL)); // Try worldInvTranspose if strange results with lighting.
		
	return output;
}

PSOut PS( PSIn input )
{
	PSOut output;
	float3 norm				= 0.5f * (input.normal + 1.0f);
	float4 bumpMap			= normalMap.Sample(m_textureSampler, input.uvCoord);
	bumpMap					= (bumpMap * 2.0f) - 1.0f;
	float3 normal			= input.normal + bumpMap.x * input.tangent + -bumpMap.y * input.binormal;
	normal					= 0.5f * (normalize(normal) + 1.0f);
	
	float4 diffuseColor = diffuse.Sample(m_textureSampler, input.uvCoord);
	
	// Remove when debugging is done.
	/*if(ninjaKick == 0)
	{
		diffuseColor.x = 1.0f;
		diffuseColor.y = 0.0f;
		diffuseColor.z = 0.0f;
	}*/
	// ------------------------------

	if(diffuseColor.w >= .5f)
		diffuseColor.w = 1.0f;
	else
		diffuseColor.w = 0.0f;

	if(diffuseColor.w == 1.0f)
	{
		output.diffuse			= float4(diffuseColor.xyz,1.0f);//input.diffuse.xyz;
		output.normal.w			= 1.0f;//input.specularPower;// 1.0f for debug.
		output.normal.xyz		= normal;//norm.xyz;
		output.wPosition.xyz	= float3(input.wpos.x, input.wpos.y, input.wpos.z);
		output.wPosition.w		= specular.Sample(m_textureSampler, input.uvCoord).x;//input.specularIntensity; // 1.0f for debug.
	}
	else // If alpha is 0. Do not blend with any previous render targets.
	{
		output.diffuse			= float4(0,0,0,0);//input.diffuse.xyz;
		output.normal.w			= 0.0f;//input.specularPower;// 1.0f for debug.
		output.normal.xyz		= float3(0.0f,0.0f,0.0f);//norm.xyz;
		output.wPosition.xyz	= float3(0,0,0);
		output.wPosition.w		= 0.0f;//specular.Sample(m_textureSampler, input.uvCoord).x;//input.specularIntensity; // 1.0f for debug.
	}
	

	return output;
}