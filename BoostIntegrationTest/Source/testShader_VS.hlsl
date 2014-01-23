float4 mainVS( float3 pos : POSITION, float3 norm : NORMAL ) : SV_POSITION
{
	return float4(pos, 1.0f);
}