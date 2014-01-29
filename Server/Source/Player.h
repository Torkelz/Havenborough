/**
 * Stuff.
 */

#pragma once

#include "User.h"

#include <Actor.h>
#include <Utilities/Util.h>

/**
 * Player contains game specific information as well as the client user.
 */
class Player
{
private:
	User::wPtr m_User;
	Actor::wPtr m_Actor;

public:
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
};
