SamplerState gBlurSampler : register (s0);
Texture2D gNormalDepthMap : register (t0);
Texture2D gInputImage : register (t1);


cbuffer cb : register (b0)
{
	float cTexelWidth;
	float cTexelHeight;
	int cHorizontalBlur;
}

struct VSOutput
{
	float4 position : SV_Position;
	float3 toFarPlane : NORMAL;
	float2 texCoord : TEXCOORD;
};

VSOutput VS(uint vID : SV_VERTEXID)
{
	VSOutput output;

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
	output.position = float4((output.texCoord * float2(2.0f,-2.0f)) + 
		float2(-1.0f,1.0f), 0.0f, 1.0f);

	return output;
}

float4 PS(VSOutput pIn) : SV_Target
{
	//static const float cWeights[11] =
	//{
	//	0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
	//};
	static const float cWeights[7] =
	{
		0.05f, 0.175f, 0.2f, 0.25f, 0.2f, 0.175f, 0.05f
	};
	static const int cBlurRadius = 3;

	float2 texOffset;
	if(cHorizontalBlur)
	{
		texOffset = float2(cTexelWidth, 0.0f);
	}
	else
	{
		texOffset = float2(0.0f, cTexelHeight);
	}

	// The center value always contributes to the sum.
	float4 color = cWeights[cBlurRadius] * gInputImage.Sample(gBlurSampler, pIn.texCoord);
	float totalWeight = cWeights[cBlurRadius];

	float4 centerNormalDepth = gNormalDepthMap.Sample(gBlurSampler, pIn.texCoord);
	centerNormalDepth.xyz = normalize((centerNormalDepth.xyz * 2.0f) - 1.0f);
	
	[unroll(cBlurRadius * 2)]
	for(int i = -cBlurRadius; i <= cBlurRadius; ++i)
	{
		// We already added in the center weight.
		if(i == 0)
			continue;

		float2 tex = pIn.texCoord + i * texOffset;

		float4 neighborNormalDepth = gNormalDepthMap.Sample(gBlurSampler, tex);

		neighborNormalDepth.xyz = normalize((neighborNormalDepth.xyz * 2.0f) - 1.0f);

		// If the center value and neighbor values differ too much (either in 
        // normal or depth), then we assume we are sampling across a discontinuity.
        // We discard such samples from the blur.
		if(dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f &&
			abs(neighborNormalDepth.a - centerNormalDepth.a) <= 20.f)
		{
			float weight = cWeights[i + cBlurRadius];

			// Add neighbor pixel to blur.
			color += weight * gInputImage.Sample(gBlurSampler, tex);
			totalWeight += weight;
		}
	}

	return color / totalWeight;
}