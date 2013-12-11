float4 main( float3 pos : POSITION, float3 norm : NORMAL ) : SV_POSITION
{
	return float4(pos, 1.0f);
}

float4 PS( float4 input : SV_POSITION) :  SV_Target0
{
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}