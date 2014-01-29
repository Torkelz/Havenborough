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
	if (p_GameType == "test")
	{
		ActorFactory::ptr actorFactory(new ActorFactory(0));

		std::shared_ptr<TestGameRound> gameRound(new TestGameRound);
		gameRound->setGameType(p_GameType);
		gameRound->initialize(actorFactory, m_ReturnLobby);

		return gameRound; 
	}
	else if (p_GameType == "serverLevel")
	{
		ActorFactory::ptr actorFactory(new ActorFactory(0));

		std::shared_ptr<FileGameRound> gameRound(new FileGameRound);
		gameRound->setFilePath("../../Client/Bin/assets/levels/Level1.2.btxl");
		gameRound->setGameType(p_GameType);
		gameRound->initialize(actorFactory, m_ReturnLobby);

		return gameRound;
	}
	else if (p_GameType == "serverDebugLevel")
	{
		ActorFactory::ptr actorFactory(new ActorFactory(0));

		std::shared_ptr<FileGameRound> gameRound(new FileGameRound);
		gameRound->setFilePath("../../Client/Bin/assets/levels/Level2.btxl");
		gameRound->setGameType(p_GameType);
		gameRound->initialize(actorFactory, m_ReturnLobby);

		return gameRound;
	}
	else
	{
		throw ServerException("Unknown game type", __LINE__, __FILE__);
	}
}
