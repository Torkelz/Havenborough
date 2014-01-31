#pragma pack_matrix(row_major)

SamplerState m_textureSampler	: register(s0);
Texture2D m_texture				: register(t0);

struct VSIn
{
	float3 position	: SV_POSITION;
	float4 color	: COLOR;
};

struct GSIn
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
};

cbuffer cb : register(b1)
{
	float4x4	view;
	float4x4	projection;
	float4		eyePosW;
	float2		size;
}


struct PSIn
{
	float4 position : SV_POSITION;
	float2 uvCoord	: COORD;
	float4 color	: COLOR;
};


GSIn VS(VSIn input)
{
	GSIn res =
	{
		float4(input.position, 1.f),
		input.color,

	};
	return res;
}

// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void GS(point GSIn gIn[1], inout TriangleStream<PSIn> triStream)
{
	//compute world matrix so that billboard faces the camera
	float3 t_position;
	t_position.x = gIn[0].position.x;
	t_position.y = gIn[0].position.y;
	t_position.z = gIn[0].position.z;

	float3 look = normalize(eyePosW.xyz - t_position);
	float3 right = normalize(cross(float3(0,1,0), look));
	float3 up = cross(look, right);

	float4x4 world = {  float4(-right.x,up.x,look.x,gIn[0].position.x),
						float4(-right.y,up.y,look.y,gIn[0].position.y),
						float4(-right.z,up.z,look.z,gIn[0].position.z),
						float4(0,0,0,1)};

	float4x4 WVP = mul(projection, mul(view, world));
	//compute 4 triangle strip vertices (quad) in local space.
	//the quad faces down the +Z axis in local space.
	float halfWidth = 0.5f*size.x;
	float halfHeight = 0.5f*size.y;
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
		gOut.position = mul(WVP, v[i]);
		gOut.uvCoord = quadUVC[i];
		gOut.color = gIn[0].color;
		triStream.Append(gOut);
	}
}

float4 PS(PSIn p_input) : SV_TARGET
{	
	float4 temp = m_texture.Sample(m_textureSampler, p_input.uvCoord);
	return temp * p_input.color;
}
