SamplerState cTextureSampler : register(s0);
Texture2D cHUD_Texture : register(t0);

cbuffer cb : register (b0)
{
	float4x4 cView;
	float4x4 cProjection;
};

struct VS_Output
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;

};

VS_Output VS(uint vID : SV_VERTEXID)
{
	VS_Output output;

	switch(vID)
	{
		case 0:	 output.texCoord = float2(0,0);	break;
		case 1:	 output.texCoord = float2(1,0);	break;
		case 2:	 output.texCoord = float2(0,1);	break;
		case 3:	 output.texCoord = float2(0,1);	break;
		case 4:	 output.texCoord = float2(1,0);	break;
		case 5:	 output.texCoord = float2(1,1);	break;
		default: output.texCoord = float2(-1,-1);break;
	}
	output.position = float4((output.texCoord * float2(2.0f, -2.0f)) + 
		float2(-1.0f, 1.0f), 0.0f, 1.0f);

	return output;
}
float4 PS(VS_Output PS_In)
{
	float2 screenPos = PS_In.texCoord;


	return float4(cHUD_Texture.Sample(cTextureSampler, screenPos).xyz, 1.0f);
}