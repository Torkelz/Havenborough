cbuffer BlockConstBuffer
{
        float4x4	view;
        float4x4	proj;
};

struct VSInput
{
        float4			m_posL : POSITION;
		float4			m_color : COLOR;
};

struct VSOutput
{
        float4 m_posH : SV_POSITION;
		float4 m_color : COLOR0;
};

VSOutput VS(VSInput p_vIn)
{
    VSOutput vOut;
        
    // Transform to homogeneous clip space.
    vOut.m_posH = mul(p_vIn.m_posL, mul(view,proj));
    vOut.m_color = p_vIn.m_color;

    return vOut;
}

float4 PS(VSOutput p_input) : SV_Target
{
    return p_input.m_color;
    //float4(1.0f, 1.0f, 1.0f, 1.0f);
}