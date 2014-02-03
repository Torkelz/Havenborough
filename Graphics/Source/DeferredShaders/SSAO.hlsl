SamplerState gRandomVectorSampler : register(s0);
SamplerState gNormalDepthSampler : register (s1);
Texture2D gNormalDepthMap : register (t0);
Texture2D gRandomVecMap : register (t1);

cbuffer FrustumFarCorners
{
	float4x4 gViewToTexSpace;
	float4 corners[4];
	float4 offsetVectors[14];
	float occlusionRadius;
	float surfaceEpsilon;
	float occlusionFadeEnd;
	float occlusionFadeStart;
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
	if(distZ > surfaceEpsilon)
	{
		float fadeLength = occlusionFadeEnd - occlusionFadeStart;

		// Linearly decrease occlusion from 1 to 0 as distZ goes 
        // from gOcclusionFadeStart to gOcclusionFadeEnd.  
		occlusion = saturate((occlusionFadeEnd - distZ) / fadeLength);
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

	output.toFarPlane = corners[index].xyz;

	return output; 
}

float4 PS(VSOutput vIn) : SV_Target
{
	int sampleCount = 14;
	// p -- the point we are computing the ambient occlusion for.
    // n -- normal vector at p.
    // q -- a random offset from p.
    // r -- a potential occluder that might occlude p.

    // Get viewspace normal and z-coord of this pixel.  The tex-coords for
    // the fullscreen quad we drew are already in uv-space.
	float4 normalDepth = gNormalDepthMap.SampleLevel(gNormalDepthSampler, vIn.texCoord, 0.0f);

	float3 n = normalDepth.xyz;
	float pz = normalDepth.w;

	// Reconstruct full view space position (x,y,z).
    // Find t such that p = t*vIn.ToFarPlane.
    // p.z = t*pin.ToFarPlane.z
    // t = p.z / pin.ToFarPlane.z
	float3 p = (pz / vIn.toFarPlane.z) * vIn.toFarPlane;

	// Extract random vector and map from [0,1] --> [-1, +1].
	float3 randVec = 2.0f * gRandomVecMap.SampleLevel(gRandomVectorSampler, 4.0f * vIn.texCoord, 0.0f).xyz - 1.0f;

	float occlusionSum = 0.0f;

	// Sample neighboring points about p in the hemisphere oriented by n.
	[unroll]
	for(int i = 0; i < sampleCount; i++)
	{
		// Are offset vectors are fixed and uniformly distributed (so that our offset vectors
        // do not clump in the same direction).  If we reflect them about a random vector
        // then we get a random uniform distribution of offset vectors.
		float3 offset = reflect(offsetVectors[i].xyz, randVec);

		// Flip offset vector if it is behind the plane defined by (p, n).
		float flip = sign(dot(offset, n));

		// Sample a point near p within the occlusion radius.
		float3 q = p + flip * occlusionRadius * offset;

		// Project q and generate projective tex-coords.
		float4 projQ = mul(float4(q, 1.0f), gViewToTexSpace);
		projQ /= projQ.w;

		// Find the nearest depth value along the ray from the eye to q (this is not
        // the depth of q, as q is just an arbitrary point near p and might
        // occupy empty space).  To find the nearest depth we look it up in the depthmap.
		float rz = gNormalDepthMap.SampleLevel(gNormalDepthSampler, projQ.xy, 0.0f).w;

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