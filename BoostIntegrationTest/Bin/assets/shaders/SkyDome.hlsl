#pragma pack_matrix(row_major)

TextureCube m_texture : register ( t0 );
SamplerState m_textureSampler : register ( s0 );

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
struct VSSceneIn
{
        float3 posL : POSITION;
};

struct PSSceneIn
{
        float4 Pos : SV_Position;
		float3 texC : TEXCOORD;
};
cbuffer cb : register(b0)
{
	float4x4 view;
	float4x4 projection;
	float3	 cameraPos;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VS(VSSceneIn input)
{
        PSSceneIn output = (PSSceneIn)0;
        float4x4 trans = {
						float4(1,0,0,cameraPos.x),
						float4(0,1,0,cameraPos.y),
						float4(0,0,1,cameraPos.z),
						float4(0,0,0,1)};
         // transform the point into view space
		output.Pos = mul( projection, mul(view, mul(trans, float4(input.posL,1.0f) ) ));
        output.texC = input.posL;
        
        return output;
}
//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PS(PSSceneIn input) : SV_Target
{        
        return m_texture.Sample(m_textureSampler, input.texC);
}