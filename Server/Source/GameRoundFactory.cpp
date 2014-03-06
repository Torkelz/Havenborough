#include "GameRoundFactory.h"

#include "ServerExceptions.h"
#include "TestGameRound.h"
#include "FileGameRound.h"

GameRoundFactory::GameRoundFactory(Lobby* p_ReturnLobby)
{
	m_ReturnLobby = p_ReturnLobby;
}

GameRound::ptr GameRoundFactory::createRound(const std::string& p_GameType)
{
	auto level = m_Levels.find(p_GameType);
	if (level != m_Levels.end())
	{
		ActorFactory::ptr actorFactory(new ActorFactory(0));

		std::shared_ptr<FileGameRound> gameRound(new FileGameRound);
		gameRound->setFilePath(level->second);
		gameRound->setGameType(level->first);
		gameRound->initialize(actorFactory, m_ReturnLobby);

		return gameRound;
	}
	else
	{
		throw ServerException("Unknown game type: " + p_GameType, __LINE__, __FILE__);
	}
}

void GameRoundFactory::addLevelPath(const std::string& p_GameType, const std::string& p_LevelPath)
{
	m_Levels[p_GameType] = p_LevelPath;
}
