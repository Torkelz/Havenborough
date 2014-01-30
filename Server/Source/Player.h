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
	 *
	 * @param p_Checkpoint Actor pointer 
	 */
	void addCheckpoint(const std::weak_ptr<Actor> p_Checkpoint);

	/**
	 * Gets the current checkpoint's body handle.
	 *
	 * @return the BodyHandle
	 */
	BodyHandle getCurrentCheckpointBodyHandle(void);

	/**
     * Checks if the finish line been reached.
	 *
     * @return true if finish line been reached, false if not
     */
    bool reachedFinishLine(void);

	/**
	 * Checks if the finish line been reached.
	 *
	 * @return true if finish line been reached, false if not
	 */
	Actor::ptr getCurrentCheckpoint(void);

	/**
	 * Remove the last item in the vector of checkpoints and set the next one.
	 */
	void changeCheckpoint();

	/**
	 * Returns the next checkpoint corresponding tone to be shaded with.
	 *
	 * @return a vector3 with color information in RGB values;
	 */
	Vector3 getCurrentCheckpointColor();
};
