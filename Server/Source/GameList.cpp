#include "GameList.h"

void GameList::addGameRound(GameRound::ptr p_Game)
{
	m_RunningGames.push_back(p_Game);
	p_Game->setup();
	p_Game->start();
}

void GameList::stopAllGames()
{
	m_RunningGames.clear();
}
