//################################
//		HELPER FUNCTIONS
//################################
void GetGBufferAttributes(in float2 screenPos, in Texture2D normalTex, in Texture2D diffuseTex,
	in Texture2D SSAO_Tex, in Texture2D wPosTex, out float3 normal, out float3 diffuseAlbedo,
	out float3 specularAlbedo, out float3 ssao, out float3 position, out float specularPower)
{
	int3 sampleIndex = int3(screenPos,0);
	float4 normalTexSample = normalTex.Load(sampleIndex).xyzw;	

	float3 normal2 = normalTexSample.xyz;
	normal = (normal2 * 2.0f) - 1.0f;
	
	float4 diffuseTexSample = diffuseTex.Load(sampleIndex).xyzw;

	diffuseAlbedo = diffuseTexSample.xyz;	
	specularPower = diffuseTexSample.w;

	ssao = SSAO_Tex.Load(sampleIndex).xyz;

	float4 wPosTexSample = wPosTex.Load(sampleIndex).xyzw;	
	position = wPosTexSample.xyz;
	specularAlbedo = wPosTexSample.w;
}
//################################
//		Shared Pixel Shader
//################################
