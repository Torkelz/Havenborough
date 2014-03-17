//################################
//		Shared Structs
//################################
struct VSLightInput
{
	float3	vposition		: POSITION;
	float3	lightPos		: LPOSITION;
    float3	lightColor		: COLOR;
	float3	lightDirection	: DIRECTION;
    float2	spotlightAngles	: ANGLE;
    float	lightRange		: RANGE;
	float	lightIntensity	: INTENSITY;
};

struct VSLightOutput
{
	float4	vposition		: SV_Position;
	float3	lightPos		: LPOSITION;
    float3	lightColor		: COLOR;
	float3	lightDirection	: DIRECTION;
    float2	spotlightAngles	: ANGLE;
    float	lightRange		: RANGE;
	float	lightIntensity	: INTENSITY;
};

//################################
//		HELPER FUNCTIONS
//################################

float4 GetGBufferDiffuseSpec(in int3 p_SampleIndex, in Texture2D p_DiffuseTex)
{
	return p_DiffuseTex.Load(p_SampleIndex);
}

float3 GetGBufferNormal(in int3 p_SampleIndex, in Texture2D p_NormalTex)
{
	return p_NormalTex.Load(p_SampleIndex).xyz * 2.f - 1.f;
}

float4 GetWorldPosition(in int3 p_SampleIndex, in Texture2D p_vPosTex)
{
	return p_vPosTex.Load(p_SampleIndex);	
}

void GetGBufferAttributes(in float2 p_ScreenPos,in float p_SSAOScale, in Texture2D p_NormalTex, in Texture2D p_DiffuseTex,
	in Texture2D p_SSAO_Tex, in Texture2D p_WPosTex, out float3 p_Normal, out float3 p_DiffuseAlbedo,
	out float3 p_SSAO, out float3 p_Position)
{
	int3 sampleIndex = int3(p_ScreenPos,0);

	p_Normal = GetGBufferNormal(sampleIndex, p_NormalTex);

	float4 diffuseTexSample = GetGBufferDiffuseSpec(sampleIndex, p_DiffuseTex);
	p_DiffuseAlbedo = diffuseTexSample.xyz;	

	int3 scaledScreenPos = int3(p_ScreenPos * p_SSAOScale, 0);
	//scaledScreenPos *= p_SSAOScale;
	p_SSAO = p_SSAO_Tex.Load(scaledScreenPos).xyz;

	float4 wPosTexSample = GetWorldPosition(sampleIndex, p_WPosTex);
	p_Position = wPosTexSample.xyz;
}