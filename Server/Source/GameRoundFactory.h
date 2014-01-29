#pragma once

#include "GameRound.h"

#include <memory>
#include <string>

class Lobby;

class GameRoundFactory
{
private:
	Lobby* m_ReturnLobby;

public:
	explicit GameRoundFactory(Lobby* p_ReturnLobby);

	GameRound::ptr createRound(const std::string& p_GameType);
};
