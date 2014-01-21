#pragma once

#include "GameRound.h"

#include <vector>

class GameList
{
private:
	std::vector<GameRound::ptr> m_RunningGames;

public:
	void addGameRound(GameRound::ptr p_Game);
	void stopAllGames();
};
