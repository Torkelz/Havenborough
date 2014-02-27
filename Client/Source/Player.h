#pragma once
#include "Actor.h"
#include "INetwork.h"
#include "IPhysics.h"

#include <DirectXMath.h>

class Player
{
private:
	IPhysics *m_Physics;
	INetwork *m_Network;
	std::weak_ptr<Actor> m_Actor;

	bool m_IsJumping;
	int m_JumpCount, m_JumpCountMax;
    float m_JumpTime, m_JumpTimeMax;
	float m_JumpForce;
	float m_MaxSpeed; // Centimeters per secound
	float m_AccConstant;
	float m_DirectionX;	// (-1 - +1)
	float m_DirectionZ;	// (-1 - +1)
	float m_ClimbOffset; // Offset to position the player correctly when climbing. Needed because the sphere is positioned under the characters center.
	DirectX::XMFLOAT3 m_GroundNormal;

	float m_CurrentMana, m_PreviousMana, m_MaxMana, m_ManaRegenerationSlow, m_ManaRegenerationFast;
	bool m_IsAtMaxSpeed, m_IsPreviousManaSet;

	bool m_ForceMove, m_Climb;
	float m_CurrentForceMoveTime;
	std::vector<DirectX::XMFLOAT2> m_ForceMoveY;
	std::vector<DirectX::XMFLOAT2> m_ForceMoveZ;
	DirectX::XMFLOAT4X4 m_ForceMoveRotation;
	DirectX::XMFLOAT3 m_ForceMoveStartPos;
	DirectX::XMFLOAT3 m_CenterReachPos;
	DirectX::XMFLOAT3 m_Side;
	DirectX::XMFLOAT3 m_EdgeOrientation;
	DirectX::XMFLOAT3 m_forward;
	std::string m_ClimbId;
	DirectX::XMFLOAT3 m_LastSafePosition;

	float m_Height; 
	float m_EyeHeight;
	bool m_AllowedToMove;
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
	* @param p_Actor the player actor
	*/
	void initialize(IPhysics *p_Physics, INetwork *p_Network, std::weak_ptr<Actor> p_Actor);
	
	/**
	* Updates the player's actions such as movement and jumping. If forced movement is active, the position will be updated between two stored positions.
	* @param p_DeltaTime the dt between two consecutive frames
	*/
	void update(float p_DeltaTime);

	/**
	* Forces the player to change its position. The player can not control the movement.
	* @param p_ClimbId the name of the current climb animation
	* @param p_CollisionNormal calculated in the physics
	* @param p_BoxPos the center position of the edge box
	* @param p_BoxOrientation the 2D orientation of the box
	*/
	virtual void forceMove(std::string p_ClimbId, DirectX::XMFLOAT3 p_CollisionNormal, DirectX::XMFLOAT3 p_BoxPos, DirectX::XMFLOAT3 p_EdgeOrientation);

	/**
	 * Sets the current mana. It isn't possible to set current mana higher than the Maximum mana and lower than 0.
	 * 
	 *
	 * @param p_Mana, new mana for the player
	 */
	void setCurrentMana(float p_Mana);
	/**
	 * Returns the current mana for the player.
	 *
	 * @return player's current mana
	 */
	float getPreviousMana();
	/**
	 * Returns the previous mana from the last frame.
	 *
	 * @return player's previous mana
	 */
	float getCurrentMana();
	/**
	 * Returns the maximum mana.
	 *
	 * @return players maximum mana
	 */
	float getMaxMana();

	/**
	 * Returns if the player is running at max speed.
	 *
	 * @return true if the player is running at max speed.
	 */
	bool getIsAtMaxSpeed();

	/**
	* Sets the position of the player at specified position in the game world.
	* @param p_Position the position where to place the player
	*/
	void setPosition(const DirectX::XMFLOAT3 &p_Position);
	
	/**
	* Gets the position of the player.
	* @return the position
	*/
	virtual DirectX::XMFLOAT3 getPosition(void) const;

	/**
	 * Get the eye position of the player.
	 *
	 * @return the position of the players eyes
	 */
	DirectX::XMFLOAT3 getEyePosition() const;

	/**
	 *
	 */
	DirectX::XMFLOAT3 getFootPosition(std::string p_Joint) const;

	/**
	 *
	 */
	DirectX::XMFLOAT3 getRightHandPosition() const;

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
	 * Sets the player's height to a wanted value.
	 * @param p_Height, the wanted height
	 */
	void setHeight(float p_Height);
	
	/**
	* Gets the body handle of the player.
	* @return the body handle
	*/
	virtual BodyHandle getBody() const;

	/**
	* Gets if the player is currently forced to change position.
	* @return true = the player has no control of movement, false = the player is able to move by itself
	*/
	virtual bool getForceMove(void);

	/**
	 * Get the current velocity of the player.
	 *
	 * @return the velocity of the player in cm in world space
	 */
	Vector3 getVelocity() const;
	Vector3 getDirection() const;

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

	/**
	 * The player's ground normal. Ground Normal equals the perpendicular vector from the surface the player is standing on.
	 * 
	 * @return the normal.
	 */
	DirectX::XMFLOAT3 getGroundNormal() const;
	/**
	 * Sets the player's ground normal. Ground Normal equals the perpendicular vector from the surface the player is standing on.
	 *
	 * @param p_Normal the new ground normal.
	 */
	void setGroundNormal(DirectX::XMFLOAT3 p_Normal);

	/**
	 * Sets the player's respawn position,  which is used when the player goes out of bounds.
	 *
	 * @param p_Position the position where the player will spawn after going to far away
	 */
	void setSpawnPosition(Vector3 p_Position);

	/**
	 * Activates the ability for the player to climb edges.
	 *
	 * @param p_State true if the player should be able to climb. false if the player should not be able to climb.
	 */
	void setClimbing(bool p_State);

	/**
	 * Activates the ability for the player to move.
	 *
	 * @param p_State true if the player should be able to move. false if the player should not be able to move.
	 */
	void setAllowedToMove(bool p_State);
	/**
	 * Gets the state if the player is allowed to move or not.
	 *
	 * @return true if the player should be able to move. false if the player should not be able to move.
	 */
	const bool getAllowedToMove() const;

private:
	void jump(float dt);
	void move(float p_DeltaTime);
};