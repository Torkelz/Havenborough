#include "GameRoundFactory.h"

#include "ServerExceptions.h"
#include "TestGameRound.h"

GameRound::ptr GameRoundFactory::createRound(const std::string& p_GameType)
{
	if (p_GameType != "test")
	{
		throw ServerException("Game round loading from file not implemented", __LINE__, __FILE__);
	}

	ActorFactory::ptr actorFactory(new ActorFactory);

	std::shared_ptr<TestGameRound> gameRound(new TestGameRound);
	gameRound->initialize(actorFactory);

	return gameRound;
}
