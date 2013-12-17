#pragma once
#include "IPhysics.h"
#include "../Utilities/Util.h"

#include <DirectXMath.h>

using namespace DirectX;

class Player
{
private:
	XMFLOAT3 m_Position;
	XMFLOAT3 m_LookDirection;
	float m_ViewRotation[2];

	BodyHandle m_PlayerBody;
	IPhysics *m_Physics;
	
	float m_JumpTime;
	bool m_IsJumping;
	float m_JumpForceTime;
	float m_JumpForce;
	float maxSpeed;
	float accConstant;
	XMFLOAT4 m_PrevForce;
	float m_DirectionX;
	float m_DirectionZ;

	bool m_ForceMove;
	float m_ForceMoveTime;
	float m_CurrentForceMoveTime;
	float m_ForceMoveSpeed;
	XMVECTOR m_ForceMoveStartPosition;
	XMVECTOR m_ForceMoveEndPosition;

public:
	/**
	* Constructor
	*/
	Player(void);
	
	/**
	* Destructor
	*/
	~Player(void);
	
	/**
	* Initialize the player.
	* @param p_Physics a pointer to the physics engine
	* @param p_Position the starting position of the player
	* @param p_LookDirection the direction the player will look at when starting
	*/
	void initialize(IPhysics *p_Physics, XMFLOAT3 p_Position, XMFLOAT3 p_LookDirection);
	
	/**
	* Gets the position of the player.
	* @return the position
	*/
	XMFLOAT3 getPosition(void) const;
	
	/**
	* Gets the body handle of the player.
	* @return the body handle
	*/
	BodyHandle getBody(void) const;

	/**
	* Makes the player jump if not already jumping.
	*/
	void setJump(void);
	
	/**
	* Sets the player's x-direction.
	*/
	void setDirectionX(float p_DirectionX);
	
	/**
	* Sets the player's z-direction.
	*/
	void setDirectionZ(float p_DirectionZ);
	
	/**
	* Gets if the player is currently forced to change position.
	* @return true = the player has no control of movement, false = the player is able to move by itself
	*/
	bool getForceMove(void);

	/**
	* Forces the player to change its position. The player can not control the movement.
	* @param p_StartPosition the starting position of the movement
	* @param p_EndPostion the position where the movement will end
	*/
	void forceMove(XMVECTOR p_StartPosition, XMVECTOR p_EndPosition);

	/**
	* Updates the player's actions such as movement and jumping. If forced movement is active, the position will be updated between two stored positions.
	* @param p_DeltaTime the dt between two consecutive frames
	*/
	void update(float p_DeltaTime);

private:
	void jump(float dt);
	void move(void);
};