#pragma pack_matrix(row_major)
SamplerState textureSampler	: register(s0);
Texture2D diffuseTex		: register(t0);

cbuffer cb : register(b0)
{
	float4x4 cView;
	float4x4 cProjection;
	float3	 cCameraPos;
};

struct VSIn
{
	float3 position	: POSITION;
	float2 size : SIZE;
	float scale : SCALE;
	float rotation : ROTATION;
};

struct PSIn
{
	float4 position : SV_POSITION;
	float2 texCoord : COORD;
};

//############################
// Shader step: Vertex Shader
//############################
VSIn VS(VSIn p_Input)
{
	return p_Input;
}

[maxvertexcount(4)]
void GS(point VSIn gIn[1], inout TriangleStream<PSIn> triStream)
{
	//compute world matrix so that billboard faces the camera
	float3 look = normalize(cCameraPos.xyz - gIn[0].position);
	float3 right = normalize(cross(float3(0, 1, 0), look));
	float3 up = cross(look, right);

	float4x4 world =
	{
		float4(-right.x, up.x,look.x, gIn[0].position.x),
		float4(-right.y, up.y,look.y, gIn[0].position.y),
		float4(-right.z, up.z,look.z, gIn[0].position.z),
		float4(0, 0, 0, 1)
	};

	float4x4 scale =
	{
		float4(gIn[0].scale, 0, 0, 0),
		float4(0, gIn[0].scale, 0, 0),
		float4(0, 0, gIn[0].scale, 0),
		float4(0, 0, 0, 1)
	};

	float4x4 rotation =
	{
		float4(cos(gIn[0].rotation), -sin(gIn[0].rotation), 0, 0),
		float4(sin(gIn[0].rotation), cos(gIn[0].rotation), 0, 0),
		float4(0, 0, 1, 0),
		float4(0, 0, 0, 1)
	};

	float4x4 WVP = mul(cProjection, mul(cView, world));
	//compute 4 triangle strip vertices (quad) in local space.
	//the quad faces down the +Z axis in local space.
	float halfWidth = 0.5f * gIn[0].size.x;
	float halfHeight = 0.5f * gIn[0].size.y;
	float4 v[4];
	v[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f);
	v[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);
	v[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);
	v[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);


	//tansform quad vertices to world space and output them as a triangle strip.
	PSIn gOut;
	//[unroll]
	float2 quadUVC[] = 
	{
		float2(0.f, 1.f),
		float2(1.f, 1.f),
		float2(0.f, 0.f),
		float2(1.f, 0.f),
	};

	for(int i = 0; i < 4; i++)
	{
		gOut.position = mul(WVP, mul(rotation, mul(scale, v[i])));
		gOut.texCoord = quadUVC[i];
		triStream.Append(gOut);
	}
}
//############################
// Shader step: Pixel Shader
//############################
float4 PS(PSIn p_Input) : SV_Target
{
	float4 color = diffuseTex.Sample(textureSampler, p_Input.texCoord);
	
	return color;
}