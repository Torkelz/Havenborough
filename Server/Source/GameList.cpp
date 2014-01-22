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

void GameList::removeGameRound(GameRound::ptr p_Game)
{
	std::lock_guard<std::mutex> lock(m_RunningGamesLock);

	auto removed = std::remove(m_RunningGames.begin(), m_RunningGames.end(), p_Game);
	m_RunningGames.erase(removed);
}

void GameList::stopAllGames()
{
	std::lock_guard<std::mutex> lock(m_RunningGamesLock);

	m_RunningGames.clear();
}

std::vector<GameRound::ptr> GameList::getRunningGames()
{
	std::lock_guard<std::mutex> lock(m_RunningGamesLock);

	return m_RunningGames;
}
