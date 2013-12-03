/**
 * File comment.
 */

#pragma once

#include <cstdint>

/**
 * Opaque Package type. Only to be used as a reference while using the network API.
 */
typedef unsigned int Package;

/**
 * Supported package types, used to prevent collisions.
 */
enum class PackageType : uint16_t
{
	RESERVED = 0,
	PLAYER_READY,
	ADD_OBJECT,
	REMOVE_OBJECT,
	UPDATE_OBJECTS,
	GAME_RESULT,
};

/**
 * Data for an add object package.
 */
struct AddObjectData
{
	float m_Position[3];
};

/**
 * Result codes for API use.
 */
enum class Result
{
	SUCCESS,
	FAILURE,
};
/**
 * Callback type for reporting that an action has been completed.
 */
typedef void (*actionDoneCallback)(Result, void* p_UserData);
