/**
 * Stuff.
 */

#pragma once

#include <IConnectionController.h>

#include <functional>
#include <memory>

/**
 * User is a client connection and state about where the user is.
 */
class User
{
public:
	/**
	 * User state representing where the user can be found.
	 */
	enum class State
	{
		CHANGING_STATE,		/// In-between states
		LOBBY,				/// In lobby, before joining game
		WAITING_FOR_GAME,	/// Joined game, waiting for more players
		IN_GAME,			/// During gameplay
		LOADING_LEVEL,		/// Still loading level
		WAITING_FOR_START,	/// Waiting for other players to load level
	};

	/**
	 * Shared pointer type.
	 */
	typedef std::shared_ptr<User> ptr;
	/**
	 * Weak pointer type.
	 */
	typedef std::weak_ptr<User> wPtr;

private:
	State m_State;
	IConnectionController* m_Connection;
	std::string m_Username;
	std::string m_CharacterName;
	std::string m_CharacterStyle;

public:
	/**
	 * contructor.
	 *
	 * @param p_Connection the client connection to wrap
	 */
	explicit User(IConnectionController* p_Connection);

	/**
	 * Get the connection for the user.
	 *
	 * @return the users connection
	 */
	IConnectionController* getConnection() const;

	/**
	 * Get the state of the user.
	 *
	 * @return the user state
	 */
	State getState() const;
	/**
	 * Set a new state.
	 *
	 * @param p_NewState the new state
	 */
	void setState(State p_NewState);

	/**
	 * Gets the name of the user.
	 *
	 * @return the name of the user
	 */
	const std::string& getUsername() const;
	/**
	 * Sets a new username for the user.
	 *
	 * @param p_Username the new username for the user
	 */
	void setUsername(const std::string& p_Username);

	/**
	 * Sets a new character name for the user.
	 *
	 * @param p_Username the new username for the user
	 */
	void setCharacterName(const std::string& p_CharacterName);

	/**
	 * Gets the character name of the user.
	 *
	 * @return the name of the user
	 */
	const std::string& getCharacterName() const;

	/**
	 * Gets the character style of the user.
	 *
	 * @return the current style of the user
	 */
	const std::string& getCharacterStyle() const;

	/**
	 * Sets a new character style for the user.
	 *
	 * @param p_Style the name of the style to use
	 */
	void setCharacterStyle(const std::string& p_Style);
};
