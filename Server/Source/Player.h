/**
 * Stuff.
 */

#pragma once

#include "User.h"

#include <Actor.h>
#include <Utilities/Util.h>
#include "CheckpointSystem.h"

/**
 * Player contains game specific information as well as the client user.
 */
class Player
{
private:
	User::wPtr m_User;
	Actor::wPtr m_Actor;
	CheckpointSystem m_CheckpointSystem;

public:

	typedef std::shared_ptr<Player> ptr;
	/**
	 * constructor.
	 *
	 * @param p_User the user to make player for
	 */
	explicit Player(User::wPtr p_User);

	/**
	 * Get the user associated with the player.
	 *
	 * @return the players user
	 */
	User::wPtr getUser() const;
	/**
	 * Release the user associated with the player.
	 * <p>
	 * Use to get rid of the player.
	 */
	void releaseUser();

	/**
	 * Get the player's actor.
	 *
	 * @return the actor of the player, may be empty
	 */
	Actor::wPtr getActor() const;

	/**
	 * Set the actor to be used for this player.
	 *
	 * @param p_Actor the new actor, may be empty
	 */
	void setActor(Actor::wPtr p_Actor);

	/**
	* Adds a checkpoint to the vector of checkpoints.
	* @param p_Checkpoint Actor pointer 
	*/
	void addCheckpoint(const std::weak_ptr<Actor> p_Checkpoint);

		/**
	* Gets the current checkpoint's body handle.
	* @return the BodyHandle
	*/
	BodyHandle getCurrentCheckpointBodyHandle(void);

	/**
	* Checks if the finish line been reached.
	* @return true if finish line been reached, false if not
	*/
	Actor::Id reachedFinishLine(void);

	/**
	* Removes the current checkpoint and sets the next checkpoint in the vector to the current checkpoint with corresponding 
	* tone to be shadede with.
	* @param p_Objects vector with Actor objects for the current moment
	*/
	Vector3 changeCheckpoint();
};
