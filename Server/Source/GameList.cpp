#include "GameList.h"

#include <algorithm>

void GameList::addGameRound(GameRound::ptr p_Game)
{
	std::lock_guard<std::mutex> lock(m_RunningGamesLock);

	m_RunningGames.push_back(p_Game);
	p_Game->setOwningList(this);
	p_Game->setup();
	p_Game->start();
}

void GameList::removeGameRound()
{
	std::lock_guard<std::mutex> lock(m_RunningGamesLock);

	auto removed = std::remove_if(m_RunningGames.begin(), m_RunningGames.end(), [] (GameRound::wPtr p_Game){ return p_Game.expired(); });
	m_RunningGames.erase(removed, m_RunningGames.end());
}

void GameList::stopAllGames()
{
	std::lock_guard<std::mutex> lock(m_RunningGamesLock);

	m_RunningGames.clear();
}

std::vector<GameRound::ptr> GameList::getRunningGames()
{
	std::lock_guard<std::mutex> lock(m_RunningGamesLock);

	std::vector<GameRound::ptr> result;
	for (auto& weakGame : m_RunningGames)
	{
		GameRound::ptr game = weakGame.lock();
		if (game)
		{
			result.push_back(game);
		}
	}

	return result;
}
