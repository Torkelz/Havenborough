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
	static const float cWeights[11] =
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
	};
	static const int cBlurRadius = 5;

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
	float4 color = cWeights[5] * gInputImage.SampleLevel(gBlurSampler, pIn.texCoord, 0.0f);
	float totalWeight = cWeights[5];

	float4 centerNormalDepth = gNormalDepthMap.SampleLevel(gBlurSampler, pIn.texCoord, 0.0f);
	float4 CND;
	CND.xyz= normalize((centerNormalDepth.xyz * 2.0f) - 1.0f);
	CND.w = centerNormalDepth.w;

	for(int i = -cBlurRadius; i <= cBlurRadius; ++i)
	{
		// We already added in the center weight.
		if(i == 0)
			continue;

		float2 tex = pIn.texCoord + i * texOffset;

		float4 neighborNormalDepth = gNormalDepthMap.SampleLevel(gBlurSampler, tex, 0.0f);
		float4 NND;
		NND.xyz = normalize((neighborNormalDepth.xyz * 2.0f) - 1.0f);
		NND.w = neighborNormalDepth.w;

		// If the center value and neighbor values differ too much (either in 
        // normal or depth), then we assume we are sampling across a discontinuity.
        // We discard such samples from the blur.
		if(dot(NND.xyz, CND.xyz) >= 0.8f &&
			abs(NND.a - CND.a) <= 20.f)
		{
			float weight = cWeights[i + cBlurRadius];

			// Add neighbor pixel to blur.
			color += weight * gInputImage.SampleLevel(gBlurSampler, tex, 0.0f);
			totalWeight += weight;
		}
	}

	return color / totalWeight;
}