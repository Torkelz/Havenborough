Texture2D debugTex				: register (t0);

SamplerState m_textureSampler	: register (s0);

//##################################
//		SHADER PASSES
//##################################
struct VSOutput
{
	float4	position : SV_Position;
	float2  texCoord : TEXCOORD;
};

VSOutput VS( uint vID : SV_VERTEXID )
{
	VSOutput output;
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

float4 PS( VSOutput input ) :  SV_Target0
{
	float2 screenPos = input.texCoord;
	//uint3 texCoord = uint3(input.texCoord, 0);
	// Sample the texture and draw it on the screen quad.
	return  float4(debugTex.Sample(m_textureSampler, screenPos).xyz, 1.0f);
	//float4 s = debugTex.Sample(m_textureSampler, screenPos).xyzw;
	//return float4(s.x,s.x,s.x, 1.f);
}