#pragma once

#include "GameRound.h"

#include <mutex>
#include <vector>

class GameList
{
private:
	std::mutex m_RunningGamesLock;
	std::vector<GameRound::ptr> m_RunningGames;

public:
	void addGameRound(GameRound::ptr p_Game);
	void removeGameRound(GameRound::ptr p_Game);
	void stopAllGames();

	std::vector<GameRound::ptr> getRunningGames();
};
