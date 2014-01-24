#include "Lobby.h"

#include "Server.h"

Lobby::Lobby(Server* p_Server)
	:	m_Server(p_Server)
{
}

void Lobby::checkFreeUsers(float p_DeltaTime)
{
	if (m_Levels.empty())
	{
		return;
	}

	for (auto& user : m_FreeUsers)
	{
		for (auto& level : m_Levels)
		{
			level.m_JoinedUsers.push_back(user);

			if (level.m_JoinedUsers.size() >= level.m_MaxPlayers)
			{
				startLevel(level);
			}

			break;
		}
	}

	m_FreeUsers.clear();

	for (auto& level : m_Levels)
	{
		if (!level.m_JoinedUsers.empty())
		{
			level.m_WaitedTime += p_DeltaTime;
		}

		if (level.m_WaitedTime > level.m_TimeoutLength)
		{
			startLevel(level);
		}
	}
}

void Lobby::addAvailableLevel(const std::string& p_LevelName, unsigned int p_MaxPlayers)
{
	AvailableLevel level =
	{
		std::vector<User::wPtr>(),
		p_MaxPlayers,
		p_LevelName,
		30.f,
		0.f
	};
	m_Levels.push_back(level);
}

void Lobby::addFreeUser(User::wPtr p_User)
{
	m_FreeUsers.push_back(p_User);
}

void Lobby::startLevel(AvailableLevel& p_Level)
{
	GameRound::ptr game = m_GameFactory.createRound(p_Level.m_LevelName);
	for (auto& player : p_Level.m_JoinedUsers)
	{
		game->addNewPlayer(player);
	}

	p_Level.m_JoinedUsers.clear();
	p_Level.m_WaitedTime = 0.f;

	m_Server->addNewGame(game);
}
