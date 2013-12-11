struct Light
{
	#include <DirectXMath.h>

	DirectX::XMFLOAT3	lightPos;
    DirectX::XMFLOAT3	lightColor;
	DirectX::XMFLOAT3	lightDirection;
    DirectX::XMFLOAT2	spotlightAngles;// X must be smaller than Y
    float				lightRange;
	int					lightType;

	Light(){}

	Light( DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT3 p_color,
		DirectX::XMFLOAT3 p_direction, DirectX::XMFLOAT2 p_angles,
		float p_range, int p_type )
	{
		lightPos = p_position;
		lightColor = p_color;
		DirectX::XMStoreFloat3(&lightDirection, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&p_direction)));
		//lightDirection = p_direction;
		spotlightAngles = p_angles;
		lightRange = p_range;
		lightType = p_type;
	}
};