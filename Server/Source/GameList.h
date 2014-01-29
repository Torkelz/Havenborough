/**
 * Stuff.
 */

#pragma once

#include "GameRound.h"

#include <mutex>
#include <vector>

/**
 * List of active games with synchronized access.
 */
class GameList
{
private:
	std::mutex m_RunningGamesLock;
	std::vector<GameRound::wPtr> m_RunningGames;

public:
	/**
	 * Add and start a new game round.
	 *
	 * @param p_Game the game to start
	 */
	void addGameRound(GameRound::ptr p_Game);

	/**
	 * Remove any stopped game rounds.
	 */
	void removeGameRound();
	/**
	 * Stop and remove all game rounds.
	 */
	void stopAllGames();

	/**
	 * Get a list of running games.
	 *
	 * @return a new list of games. Make sure not to keep the list
	 * around for to long, as that would postpone removal of games during that time.
	 */
	std::vector<GameRound::ptr> getRunningGames();
};
