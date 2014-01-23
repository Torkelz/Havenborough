#pragma once
#include "../Utilities/XMFloatUtil.h"

struct Light
{
public:
	enum class Type
	{
		DIRECTIONAL,
		POINT,
		SPOT
	};

	Vector3	position;
    Vector3	color;
	Vector3	direction;
    Vector2	spotlightAngles;// X must be smaller than Y
    float range;
	Type type;

	typedef unsigned int Id;
	Id id;

	Light()
	{
		position = Vector3(0.f,0.f,0.f);
		color = Vector3(0.f,0.f,0.f);
		direction = Vector3(0.f,0.f,0.f);
		spotlightAngles = Vector2(0.f,0.f);
		range = 0.f;
		type = Type::DIRECTIONAL;
	}

	static Light createDirectionalLight(Vector3 p_Direction, Vector3 p_Color)
	{
		Light light;
		light.direction = p_Direction;
		light.color = p_Color;
		light.type = Type::DIRECTIONAL;

		return light;
	}

	static Light createSpotLight(Vector3 p_Position, Vector3 p_Direction, Vector2 p_LightAngle, float p_Range, Vector3 p_Color)
	{
		Light light;
		light.position = p_Position;
		light.direction = p_Direction;
		light.spotlightAngles = p_LightAngle;
		light.range = p_Range;
		light.color = p_Color;
		light.type = Type::SPOT;

		return light;
	}

	static Light createPointLight(Vector3 p_Position, float p_Range, Vector3 p_Color)
	{
		Light light;
		light.position = p_Position;
		light.range = p_Range;
		light.color = p_Color;
		light.type = Type::POINT;

		return light;
	}

private:
	Light(Type p_Type, Vector3 p_position, Vector3 p_color,
		Vector3 p_direction, Vector2 p_angles,
		float p_range)
	{
		type = p_Type;
		position = p_position;
		color = p_color;
		direction = p_direction;
		spotlightAngles = p_angles;
		range = p_range;
		id = 0;
	}
};