#pragma pack_matrix(row_major)

//################################
//		Shared Structs
//################################
struct PSIn
{
	float4 position	: SV_POSITION;
	float4 wposition: WSPOSITION;
	float3 normal	: NORMAL;
	float2 uvCoord	: COORD;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
	float depth		: DEPTH;
};

struct PSOut
{
	half4 diffuse	: SV_Target0; // xyz = diffuse color, w = specularPower
	half4 normal	: SV_Target1; // xyz = normal.xyz, w = depth
	half4 wPosition	: SV_Target2; // xyz = world position, w = specular intensity
};

//################################
//		Shared Pixel Function
//################################
PSOut PSFunction(PSIn p_Input, float4x4 p_View, Texture2D p_DiffuseTex, Texture2D p_NormalTex, 
	Texture2D p_SpecularTex, SamplerState p_TextureSampler)
{
	PSOut output;
	float3 norm		= 0.5f * (p_Input.normal + 1.0f);
	float4 bumpMap	= p_NormalTex.Sample(p_TextureSampler, p_Input.uvCoord);
	bumpMap			= (bumpMap * 2.0f) - 1.0f;
	float3 normal	= bumpMap.x * p_Input.tangent + -bumpMap.y * p_Input.binormal + bumpMap.z * p_Input.normal;
	normal			= mul((float3x3)p_View, normal);
	normal			= 0.5f * (normalize(normal) + 1.0f);

	float4 diffuseColor = p_DiffuseTex.Sample(p_TextureSampler, p_Input.uvCoord);

	if(diffuseColor.w >= 0.7f)
		diffuseColor.w = 1.0f;
	else
		diffuseColor.w = 0.0f;

	if(diffuseColor.w == 1.0f)
	{
		output.diffuse			= float4(diffuseColor.xyz, 1.0f);//input.diffuse.xyz; //specular intensity = 1.0f
		output.normal.w			= p_Input.depth;
		output.normal.xyz		= normal;//normalize(mul((float3x3)view, normal));
		output.wPosition.xyz	= float3(p_Input.wposition.x, p_Input.wposition.y, p_Input.wposition.z);
		output.wPosition.w		= p_SpecularTex.Sample(p_TextureSampler, p_Input.uvCoord).x;

	}
	else // If alpha is 0. Do not blend with any previous render targets.
		discard;

	return output;
}