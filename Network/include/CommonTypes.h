#pragma once

#include <cstdint>

enum class PackageType : uint16_t
{
	RESERVED = 0,
	PLAYER_READY,
	ADD_OBJECT,
	REMOVE_OBJECT,
	UPDATE_OBJECTS,
	GAME_RESULT,
};

struct AddObjectData
{
	float m_Position[3];
};
