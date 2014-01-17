#pragma once
#include "IPhysics.h"
#include "../Utilities/Util.h"

#include <DirectXMath.h>

using namespace DirectX;

class Player
{
private:
	XMFLOAT3 m_Position;	// cm
	XMFLOAT3 m_LookDirection;
	float m_ViewRotation[2];

	BodyHandle m_PlayerBody;
	IPhysics *m_Physics;
	
	float m_JumpTime;
	bool m_IsJumping;
	float m_JumpForceTime;
	float m_JumpForce;
	float m_MaxSpeed; // Centimeters per secound
	float m_AccConstant;
	XMFLOAT4 m_PrevForce;	// kg * m/s^2
	float m_DirectionX;	// (-1 - +1)
	float m_DirectionZ;	// (-1 - +1)

	bool m_ForceMove;
	float m_ForceMoveTime;
	float m_CurrentForceMoveTime;
	float m_ForceMoveSpeed;	// cm/s
	Vector3 m_ForceMoveStartPosition;	// cm
	Vector3 m_ForceMoveEndPosition;	// cm

	//May not be temporary. Currently we need to know how long a character is to be able to offset it correctly
	//while climbing objects.
	float m_TempHeight; 
	float m_KneeHeight;	// cm
	float m_EyeHeight;

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
	* @param p_Position the starting position of the player in cm
	* @param p_LookDirection the direction the player will look at when starting
	*/
	void initialize(IPhysics *p_Physics, XMFLOAT3 p_Position, XMFLOAT3 p_LookDirection);
	
	/**
	* Sets the position of the player at specified position in the game world.
	* @param p_Position the position where to place the player
	*/
	void setPosition(const XMFLOAT3 &p_Position);
	
	/**
	* Gets the position of the player.
	* @return the position
	*/
	XMFLOAT3 getPosition(void) const;

	/**
	 * Get the eye position of the player.
	 *
	 * @return the position of the players eyes
	 */
	XMFLOAT3 getEyePosition() const;

	/**
	 * Get the position that would be on the ground if the player would be standing on a flat surface.
	 *
	 * @return the ground position of the player
	 */
	XMFLOAT3 getGroundPosition() const;

	/**
	 * Get the center position of the players collision volume.
	 *
	 * @return the collision volume center in cm, in world coordinates.
	 */
	XMFLOAT3 getCollisionCenter() const;

	/**
	* Gets the height of the player.
	* @return the height
	*/
	float getHeight(void) const;
	
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
	void forceMove(Vector3 p_StartPosition, Vector3 p_EndPosition);

	/**
	* Updates the player's actions such as movement and jumping. If forced movement is active, the position will be updated between two stored positions.
	* @param p_DeltaTime the dt between two consecutive frames
	*/
	void update(float p_DeltaTime);

	/**
	 * Get the current velocity of the player.
	 *
	 * @return the velocity of the player in cm in world space
	 */
	Vector3 getVelocity() const;

private:
	void jump(float dt);
	void move(void);
};