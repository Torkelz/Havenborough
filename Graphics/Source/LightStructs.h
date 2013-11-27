struct PointLight
{
	#include <DirectXMath.h>
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 diffuse;
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 specular;
    DirectX::XMFLOAT4 att;
    float range;

	PointLight(){}

	PointLight( DirectX::XMFLOAT4 p_position, DirectX::XMFLOAT4 p_diffuse,
		DirectX::XMFLOAT4 p_ambient, DirectX::XMFLOAT4 p_specular,
		DirectX::XMFLOAT4 p_att, float p_range )
	{
		position = p_position;
		diffuse = p_diffuse;
		ambient = p_ambient;
		specular = p_specular;
		att = p_att;
		range = p_range;
	}
};