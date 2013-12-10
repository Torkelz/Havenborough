struct Light
{
	#include <DirectXMath.h>

	DirectX::XMFLOAT3	lightPos;
	int padd1;
    DirectX::XMFLOAT3	lightColor;
	int padd2;
	DirectX::XMFLOAT3	lightDirection;
	int padd3;
    DirectX::XMFLOAT2	spotlightAngles;
    float				lightRange;
	int					lightType;

	Light(){}

	Light( DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT3 p_color,
		DirectX::XMFLOAT3 p_direction, DirectX::XMFLOAT2 p_angles,
		float p_range, int p_type )
	{
		lightPos = p_position;
		lightColor = p_color;
		lightDirection = p_direction;
		spotlightAngles = p_angles;
		lightRange = p_range;
		lightType = p_type;
	}
};