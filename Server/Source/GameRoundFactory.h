/**
 * Stuff.
 */

#pragma once

#include "GameRound.h"

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
};
