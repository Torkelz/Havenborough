#pragma pack_matrix(row_major)

SamplerState m_textureSampler	: register(s0);
Texture2D diffuse				: register(t0);

cbuffer cb : register(b1)
{
	float4x4 view;
	float4x4 projection;
	float3 cameraPos;
}

cbuffer cbWorld : register(b2)
{
	float4x4 world;

}

struct VSIn
{
	float4 centerPos: POSITION;
	float4 eyePos	: EYEPOSITION;
	float2 uvCoord	: COORD;
	float2 size		: SIZE;
};

struct PSIn
{
	float4 centerPos: SV_POSITION;
	float4 eyePos	: EYEPOSITION;
	float2 uvCoord	: COORD;
	float4 color	: COLOR;
	float2 size		: SIZE;
};

struct GSOut
{
	float4 centerPos: SV_POSITION;
	float2 uvCoord	: COORD;
};

PSIn VS(VSIn input)
{
	PSIn output;
	
	output.centerpos = mul(projection, mul(view, mul(world, input.centerpos)));
	output.uvCoord = input.uvCoord;
	output.eyepos = mul(view, input.eyepos);
	output.wpos = mul(world, input.centerpos);

	return output;
}

/*[maxvertexcount(4)]
GSIn GS(point VSIn input[1], inout TriangleStream<GSOut> outputStream)
{
	float2 vertex0 = input[0].centerpos + mul(float4(float4(-1.0*sizeX, -1.0*sizeY)), world); // 0 ---lower left front
	float2 vertex1 = input[0].centerpos + mul(float4(float4( 1.0*sizeX, -1.0*sizeY)), world); // 1 +-- LowerRightFront
	float2 vertex2 = input[0].centerpos + mul(float4(float4(-1.0*sizeX,  1.0*sizeY)), world); // 2 -+- UpperLeftFront
	float2 vertex3 = input[0].centerpos + mul(float4(float4( 1.0*sizeX,  1.0*sizeY)), world); // 3 ++- UpperRightFront
	
	GSOut outVertex = (GSOut)0;
	outVertex.centerpos = vertex3;
	outVertex.uvCoord = float2(0.0f, 1.0f);
	p_outputStream.Append(outVertex);

	outVertex.centerpos = vert1;
	outVertex.uvCoord = float2(1.0f, 1.0f);
	p_outputStream.Append(outVertex);

	outVertex.centerpos = vert2;
	outVertex.uvCoord = float2(0.0f, 0.0f);
	p_outputStream.Append(outVertex);

	outVertex.centerpos = vert0;
	outVertex.uvCoord = float2(1.0f, 0.0f);
	p_outputStream.Append(outVertex);

	p_outputStream.RestartStrip();
}*/

[maxvertexcount(4)]
GSIn GS(point VSIn input[1], inout TriangleStream<PSIn> triangleStream)
{
	// We need to create a matrix for the local coordinate system for the billboard of the given particle.
    // One axis points from the particle to the camera, one axis is the camera's side axis (for example to
    // the left) and the third one is perpendicular to both.
    GSOut outVertex = GSOut[0];

	float3 zAxis = normalize(eyepos - outVertex.centerpos);
	float3 xAxis = normalize(cross(float3(0,1,0), zAxis));
	float3 yAxis = cross(zAxis, xAxis);

	// The matrix to describe the local coordinate system is easily constructed:
	float4x4 localToWorld;
	localToWorld._11 = xAxis.x;
    localToWorld._21 = xAxis.y;
    localToWorld._31 = xAxis.z;
    localToWorld._12 = yAxis.x;
    localToWorld._22 = yAxis.y;
    localToWorld._32 = yAxis.z;
    localToWorld._13 = zAxis.x;
    localToWorld._23 = zAxis.y;
    localToWorld._33 = zAxis.z;
	localToWorld._14 = outVertex.centerpos.x;
    localToWorld._24 = outVertex.centerpos.y;
    localToWorld._34 = outVertex.centerpos.z;
    localToWorld._41 = 0;
    localToWorld._42 = 0;
    localToWorld._43 = 0;
    localToWorld._44 = 1;

	// And the matrix to transform from local to screen space.
    float4x4 transform = localToWorld * world * view * projection;

	// The positions of that quad is easily described in the local coordinate system:
    // -z points towards the camera, y points upwards and x towards the right.
    // The position marks the center of the quad, hence (0, 0, 0) is the center of the quad in
    // local coordinates and the quad has an edge-length of particle.Size to either side.
    PSIn v1, v2, v3, v4;

    float size = 0.5f; //????
    v1.centerpos = mul(float4(-size, size, 0, 1), transform);
    v1.uvCoord = float2(0, 0);
    v1.color    = triangleStream.color;
    v2.centerpos = mul(float4(size, size, 0, 1), transform);
    v2.uvCoord = float2(1, 0);
    v2.color    = triangleStream.color;
    v3.centerpos = mul(float4(-size,-size, 0, 1), transform);
    v3.uvCoord = float2(0, 1);
    v3.color    = triangleStream.color;
    v4.centerpos = mul(float4(size, -size, 0, 1), transform);
    v4.uvCoord = float2(1, 1);
    v4.color    = triangleStream.color;

    triangleStream.Append(v1);
    triangleStream.Append(v2);
    triangleStream.Append(v3);
    triangleStream.Append(v4);
}

PSOut PS(PSIn input)
{	
	input.color = diffuse.Sample(m_textureSampler, input.uvCoord);
	if(input.color.w == 0.f)
		discard;

	return output;
}
