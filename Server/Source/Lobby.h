/**
 * Stuff.
 */

#pragma once

#include "GameRoundFactory.h"
#include "User.h"

#include <mutex>
#include <vector>

class Server;

/**
 * Lobby funnels users to new game rounds.
 */
class Lobby
{
public:
	/**
	 * A description of a game that can be joined.
	 */
	struct AvailableLevel
	{
		std::vector<User::wPtr> m_JoinedUsers;
		unsigned int m_MaxPlayers;
		std::string m_LevelName;
		float m_TimeoutLength;
		float m_WaitedTime;
	};

private:
	Server* m_Server;

	std::vector<AvailableLevel> m_Levels;
	std::mutex m_UserLock;
	std::vector<User::wPtr> m_FreeUsers;
	GameRoundFactory m_GameFactory;

public:
	/**
	 * constructor.
	 *
	 * @param p_Server the owning server that handles started games
	 */
	explicit Lobby(Server* p_Server);

	/**
	 * Deal with any users in the lobby, checking if they join a game, and stuff.
	 *
	 * @param p_DeltaTime the time since last check
	 */
	void checkFreeUsers(float p_DeltaTime);

	/**
	 * Add a new level to the list of available levels.
	 *
	 * @param p_LevelName the name used to identify the level
	 * @param p_MaxPlayers the maximum number of players that
	 *				can connect before the game should start
	 */
	void addAvailableLevel(const std::string& p_LevelName, unsigned int p_MaxPlayers);
	/**
	 * Add an user to the lobby.
	 *
	 * @param p_User the user to add
	 */
	void addFreeUser(User::wPtr p_User);

private:
	void joinLevel(User::ptr p_User, const std::string& p_LevelName);
	void startLevel(AvailableLevel& p_Level);
	void handlePackages();
	void handlePackagesForOneUser(User::wPtr p_User);
};
