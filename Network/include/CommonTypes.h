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
	CREATE_OBJECTS,
	REMOVE_OBJECTS,
	UPDATE_OBJECTS,
	GAME_RESULT,
	OBJECT_ACTION,
	ASSIGN_PLAYER,
	PLAYER_CONTROL,
	DONE_LOADING,
};

struct ObjectInstance
{
	float m_Position[3];
	float m_Rotation[3];
	uint16_t m_DescriptionIdx;
	uint16_t m_Id;
};

struct UpdateObjectData
{
	float m_Position[3];
	float m_Velocity[3];
	float m_Rotation[3];
	float m_RotationVelocity[3];
	uint16_t m_Id;
	uint16_t _padding_;
};

struct PlayerControlData
{
	float m_Velocity[3];
	float m_Rotation[3];
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
