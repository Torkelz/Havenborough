float4 mainVS( float3 pos : POSITION, float3 norm : NORMAL ) : SV_POSITION
{
	return float4(pos, 1.0f);
}

float4 mainPS( float4 pos : SV_POSITION ) : SV_TARGET
{
	return pos;
}