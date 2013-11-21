float4 main( float3 pos : POSITION, float3 norm : NORMAL ) : SV_POSITION
{
	return float4(pos, 1.0f);
}