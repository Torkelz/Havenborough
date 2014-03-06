#pragma pack_matrix(row_major)

SamplerState gNormalDepthSampler : register (s0);
SamplerState gRandomVectorSampler : register(s1);
Texture2D gNormalDepthMap : register (t0);
Texture2D gRandomVecMap : register (t1);

cbuffer cb : register(b0)
{
	float4x4	cView;
	float4x4	cProjection;
	float3		cCameraPos;
	float		cSSAOScale;
};

cbuffer ConstantData : register (b1)
{
	float4 cCorners[4];
	float4 cOffsetVectors[14];
	float cOcclusionRadius;
	float cSurfaceEpsilon;
	float cOcclusionFadeEnd;
	float cOcclusionFadeStart;
}

struct VSOutput
{
	float4 position : SV_Position;
	float3 toFarPlane : NORMAL;
	float2 texCoord : TEXCOORD;
};

float OcclusionFunction(float distZ)
{
	//
    // If depth(q) is "behind" depth(p), then q cannot occlude p.  Moreover, if 
    // depth(q) and depth(p) are sufficiently close, then we also assume q cannot
    // occlude p because q needs to be in front of p by Epsilon to occlude p.
    //
    // We use the following function to determine the occlusion.  
    // 
    //
    //       1.0     -------------\
    //               |           |  \
    //               |           |    \
    //               |           |      \ 
    //               |           |        \
    //               |           |          \
    //               |           |            \
    //  ------|------|-----------|-------------|---------|--> zv
    //        0     Eps          z0            z1        
    //

	float occlusion = 0.0f;
	if(distZ > cSurfaceEpsilon)
	{
		float fadeLength = cOcclusionFadeEnd - cOcclusionFadeStart;

		// Linearly decrease occlusion from 1 to 0 as distZ goes 
        // from gOcclusionFadeStart to gOcclusionFadeEnd.  
		occlusion = saturate((cOcclusionFadeEnd - distZ) / fadeLength);
	}

	return occlusion;
}

VSOutput VS( uint vID : SV_VERTEXID )
{
	VSOutput output;
	int index;

	switch(vID)
	{
		case 0:
		{
			output.texCoord = float2(0,0);
			index = 1;
			break;
		}
		case 1:
		{
			output.texCoord = float2(1,0);
			index = 2;
			break;
		}
		case 2:
		{
			output.texCoord = float2(0,1);
			index = 0;
			break;
		}
		case 3:
		{
			output.texCoord = float2(0,1);
			index = 0;
			break;
		}
		case 4:
		{
			output.texCoord = float2(1,0);
			index = 2;
			break;
		}
		case 5:
		{
			output.texCoord = float2(1,1);
			index = 3;
			break;
		}
		default:
		{
			output.texCoord = float2(-1,-1);
			index = -1;
			break;
		}
	}
	output.position = float4((output.texCoord * float2(2.0f,-2.0f)) + 
								float2(-1.0f,1.0f), 0.0f, 1.0f);

	output.toFarPlane = cCorners[index].xyz;

	return output; 
}

float4 PS(VSOutput pIn) : SV_Target
{
	float4x4 t =
	{
		float4(0.5f, 0.0f, 0.0f, 0.5f),
		float4(0.0f, -0.5f, 0.0f, 0.5f),
		float4(0.0f, 0.0f, 1.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
	};

	float4x4 viewToTexSpace = mul(t, cProjection);

	const int sampleCount = 14;
	// p -- the point we are computing the ambient occlusion for.
    // n -- normal vector at p.
    // q -- a random offset from p.
    // r -- a potential occluder that might occlude p.

    // Get viewspace normal and z-coord of this pixel.  The tex-coords for
    // the fullscreen quad we drew are already in uv-space.

	float4 normalDepth = gNormalDepthMap.Sample(gNormalDepthSampler, pIn.texCoord);
	
	float3 n = normalize((normalDepth.xyz * 2.0f) - 1.0f);
	float pz = normalDepth.w;

	// Reconstruct full view space position (x,y,z).
    // Find t such that p = t*vIn.ToFarPlane.
    // p.z = t*pin.ToFarPlane.z
    // t = p.z / pin.ToFarPlane.z
	float3 p = (pz / pIn.toFarPlane.z) * pIn.toFarPlane;

	// Extract random vector and map from [0,1] --> [-1, +1].
	float3 randVec = 2.f * gRandomVecMap.Sample(gRandomVectorSampler, 4.f * pIn.texCoord).xyz - 1.f;

	float occlusionSum = 0.0f;

	// Sample neighboring points about p in the hemisphere oriented by n.
	[unroll]
	for(int i = 0; i < sampleCount; i++)
	{
		// Are offset vectors are fixed and uniformly distributed (so that our offset vectors
        // do not clump in the same direction).  If we reflect them about a random vector
        // then we get a random uniform distribution of offset vectors.
		float3 offset = reflect(cOffsetVectors[i].xyz, randVec);

		// Flip offset vector if it is behind the plane defined by (p, n).
		float flip = sign(dot(offset, n));

		// Sample a point near p within the occlusion radius.
		float3 q = p + flip * cOcclusionRadius * offset;

		// Project q and generate projective tex-coords.
		float4 projQ = mul(viewToTexSpace, float4(q, 1.0f));
		projQ /= projQ.w;

		// Find the nearest depth value along the ray from the eye to q (this is not
        // the depth of q, as q is just an arbitrary point near p and might
        // occupy empty space).  To find the nearest depth we look it up in the depthmap.
		float rz = gNormalDepthMap.Sample(gNormalDepthSampler, projQ.xy).w;

		// Reconstruct full view space position r = (rx,ry,rz).  We know r
        // lies on the ray of q, so there exists a t such that r = t*q.
        // r.z = t*q.z ==> t = r.z / q.z
		float3 r = (rz / q.z) * q;

		//
        // Test whether r occludes p.
        //   * The product dot(n, normalize(r - p)) measures how much in front
        //     of the plane(p,n) the occluder point r is.  The more in front it is, the
        //     more occlusion weight we give it.  This also prevents self shadowing where 
        //     a point r on an angled plane (p,n) could give a false occlusion since they
        //     have different depth values with respect to the eye.
        //   * The weight of the occlusion is scaled based on how far the occluder is from
        //     the point we are computing the occlusion of.  If the occluder r is far away
        //     from p, then it does not occlude it.
        //
		float distZ = p.z - r.z;
		float dp = max(dot(n, normalize(r - p)), 0.0f);
		float occlusion = dp * OcclusionFunction(distZ);

		occlusionSum += occlusion;
	}

	occlusionSum /= sampleCount;

	float access = 1.0f - occlusionSum;

	// Sharpen the contrast of the SSAO map to make the SSAO affect more dramatic.
	return saturate(access * access * access * access);
}