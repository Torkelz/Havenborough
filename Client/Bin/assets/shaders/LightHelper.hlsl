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
};

struct VSLightOutput
{
	float4	vposition		: SV_Position;
	float3	lightPos		: LPOSITION;
    float3	lightColor		: COLOR;
	float3	lightDirection	: DIRECTION;
    float2	spotlightAngles	: ANGLE;
    float	lightRange		: RANGE;
};

//################################
//		HELPER FUNCTIONS
//################################
void GetGBufferAttributes(in float2 p_ScreenPos, in Texture2D p_NormalTex, in Texture2D p_DiffuseTex,
	in Texture2D p_SSAO_Tex, in Texture2D p_WPosTex, out float3 p_Normal, out float3 p_DiffuseAlbedo,
	out float3 p_SpecularAlbedo, out float3 p_SSAO, out float3 p_Position, out float p_SpecularPower)
{
	int3 sampleIndex = int3(p_ScreenPos,0);
	float4 normalTexSample = p_NormalTex.Load(sampleIndex).xyzw;	

	p_Normal = (normalTexSample.xyz * 2.0f) - 1.0f;
	
	float4 diffuseTexSample = p_DiffuseTex.Load(sampleIndex).xyzw;

	p_DiffuseAlbedo = diffuseTexSample.xyz;	
	p_SpecularPower = diffuseTexSample.w;

	p_SSAO = p_SSAO_Tex.Load(sampleIndex).xyz;

	float4 wPosTexSample = p_WPosTex.Load(sampleIndex).xyzw;	
	p_Position = wPosTexSample.xyz;
	p_SpecularAlbedo = wPosTexSample.w;
}