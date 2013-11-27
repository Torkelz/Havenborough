cbuffer BlockConstBuffer : register(b0)
{
        matrix	view;
        matrix	proj;
};

struct VSInput
{
        float4			m_posL : POSITION;
		float4			m_color : COLOR;
};

struct VSOutput
{
        float4 m_posH : SV_POSITION;
		float4 m_color : COLOR;
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
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
}