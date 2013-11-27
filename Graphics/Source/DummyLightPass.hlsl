/**
  * Structs
  *
  * Inputs
  *
  * Codez
  */
struct PointLight
{
        float4 position;
        float4 diffuse;
        float4 ambient;
        float4 specular;
        float4 att;
        float range;
};

Texture2D<float4> depth : register (t0);
Texture2D<float4> normal : register (t1);
StructuredBuffer<PointLight> pointLights : register (t2);

float4 VS()
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

float4 PS() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}