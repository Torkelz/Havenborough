#pragma once
#include "Actor.h"
#include "IPhysics.h"

#include <DirectXMath.h>

class Player
{
private:
	enum class ForwardAnimationState
	{
		IDLE,
		WALKING_FORWARD,
		RUNNING_FORWARD,
		WALKING_BACKWARD,
		RUNNING_BACKWARD,
	};

	enum class SideAnimationState
	{
		IDLE,
		WALKING_LEFT,
		RUNNING_LEFT,
		WALKING_RIGHT,
		RUNNING_RIGHT,
	};

	enum class JumpAnimationState
	{
		IDLE,
		JUMP,
		FLYING,
		FALLING,
		LIGHT_LANDING,
		HARD_LANDING,
	};

	ForwardAnimationState m_PrevForwardState;
	SideAnimationState m_PrevSideState;
	JumpAnimationState m_PrevJumpState;
	float m_FallSpeed;

	DirectX::XMFLOAT3 m_LookDirection;
	float m_ViewRotation[2];

	IPhysics *m_Physics;
	std::weak_ptr<Actor> m_Actor;

	bool m_IsJumping;
	int m_JumpCount, m_JumpCountMax;
	float m_JumpDelay, m_JumpDelayMax;
    float m_JumpTime, m_JumpTimeMax;
	float m_JumpForce;
	float m_MaxSpeed; // Centimeters per secound
	float m_AccConstant;
	DirectX::XMFLOAT3 m_PrevForce;	// kg * m/s^2
	float m_DirectionX;	// (-1 - +1)
	float m_DirectionZ;	// (-1 - +1)

	bool m_ForceMove;
	float m_ForceMoveTime;
	float m_CurrentForceMoveTime;
	float m_ForceMoveSpeed;	// cm/s
	//Vector3 m_ForceMoveStartPosition;	// cm
	//Vector3 m_ForceMoveEndPosition;	// cm
	std::vector<DirectX::XMUINT2> m_ForceMoveY;
	std::vector<DirectX::XMUINT2> m_ForceMoveZ;
	//DirectX::XMFLOAT3 m_ForceMoveNormal;
	DirectX::XMFLOAT4X4 m_ForceMoveRotation;
	DirectX::XMFLOAT3 m_ForceMoveStartPos;


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
	void initialize(IPhysics *p_Physics, DirectX::XMFLOAT3 p_LookDirection, std::weak_ptr<Actor> p_Actor);
	
	/**
	* Sets the position of the player at specified position in the game world.
	* @param p_Position the position where to place the player
	*/
	void setPosition(const DirectX::XMFLOAT3 &p_Position);
	
	/**
	* Gets the position of the player.
	* @return the position
	*/
	DirectX::XMFLOAT3 getPosition(void) const;

	/**
	 * Get the eye position of the player.
	 *
	 * @return the position of the players eyes
	 */
	DirectX::XMFLOAT3 getEyePosition() const;

	/**
	 * Get the position that would be on the ground if the player would be standing on a flat surface.
	 *
	 * @return the ground position of the player
	 */
	DirectX::XMFLOAT3 getGroundPosition() const;

	/**
	 * Get the center position of the players collision volume.
	 *
	 * @return the collision volume center in cm, in world coordinates.
	 */
	DirectX::XMFLOAT3 getCollisionCenter() const;

	/**
	* Gets the height of the player.
	* @return the height
	*/
	float getHeight(void) const;
	float getChestHeight(void) const;
	float getWaistHeight(void) const;
	float getKneeHeight(void) const;
	
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
	void forceMove(std::string p_ClimbId, DirectX::XMFLOAT3 p_CollisionNormal);

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
	Vector3 getDirection() const;

	/**
	 * Get the actor that represents the player.
	 *
	 * @return an actor, or an empty pointer if none exists
	 */
	std::weak_ptr<Actor> getActor() const;
	/**
	 * Set a new actor to represent the player.
	 *
	 * @param p_Actor a valid actor, or an empty pointer to remove
	 */
	void setActor(std::weak_ptr<Actor> p_Actor);

private:
	void jump(float dt);
	void move(void);
};