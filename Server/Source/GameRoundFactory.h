/**
 * Stuff.
 */

#pragma once

#include "GameRound.h"

#include <map>
#include <memory>
#include <string>

class Lobby;

/**
 * GameRoundFactory produces game rounds from a type.
 */
class GameRoundFactory
{
private:
	Lobby* m_ReturnLobby;

	std::map<std::string, std::string> m_Levels;

public:
	/**
	 * constructor.
	 *
	 * @param p_ReturnLobby the lobby where game rounds should send leaving players
	 */
	explicit GameRoundFactory(Lobby* p_ReturnLobby);

	/**
	 * Create a new round of a specific type.
	 *
	 * @param p_GameType the name of the game type to create
	 * @return a newly created game round
	 */
	GameRound::ptr createRound(const std::string& p_GameType);

	/**
	 * Adds a binding between a level name and a level path.
	 *
	 * @param p_LevelName the name of the level that is later used with #createRound
	 * @param p_LevelPath the path to the level file describing a game round
	 */
	void addLevelPath(const std::string& p_GameType, const std::string& p_LevelPath);
};
