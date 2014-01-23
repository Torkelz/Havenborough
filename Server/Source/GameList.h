#pragma once

#include "GameRound.h"

#include <mutex>
#include <vector>

class GameList
{
private:
	std::mutex m_RunningGamesLock;
	std::vector<GameRound::wPtr> m_RunningGames;

public:
	void addGameRound(GameRound::ptr p_Game);
	void removeGameRound();
	void stopAllGames();

	std::vector<GameRound::ptr> getRunningGames();
};
