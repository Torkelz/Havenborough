/**
 * Stuff.
 */

#pragma once

#include "User.h"
#include "../../Client/Utilities/Util.h"

/**
 * Player contains game specific information as well as the client user.
 */
class Player
{
public:
	struct Box
	{
		uint16_t actorId;

		Vector3 position;
		Vector3 velocity;
		Vector3 rotation;
		Vector3 rotationVelocity;
	};
	Box m_PlayerBox;

private:
	User::wPtr m_User;

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
};
