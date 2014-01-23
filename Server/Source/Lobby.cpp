#include "Lobby.h"

#include "Server.h"
#include "../../Client/Source/Logger.h"

#include <algorithm>

Lobby::Lobby(Server* p_Server)
	:	m_Server(p_Server),
		m_GameFactory(this)
{
}

void Lobby::checkFreeUsers(float p_DeltaTime)
{
	handlePackages();

	auto removeIt = std::remove_if(m_FreeUsers.begin(), m_FreeUsers.end(),
		[] (User::wPtr p_User)
		{
			User::ptr user = p_User.lock();
			return !user || user->getState() != User::State::LOBBY;
		});
	m_FreeUsers.erase(removeIt, m_FreeUsers.end());

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
		5.f,
		0.f
	};
	m_Levels.push_back(level);
}

void Lobby::addFreeUser(User::wPtr p_User)
{
	User::ptr user = p_User.lock();
	if (user)
	{
		user->setState(User::State::LOBBY);

		m_FreeUsers.push_back(p_User);
	}
}

void Lobby::joinLevel(User::ptr p_User, const std::string& p_LevelName)
{
	for (auto& level : m_Levels)
	{
		if (level.m_LevelName == p_LevelName)
		{
			level.m_JoinedUsers.push_back(p_User);
			p_User->setState(User::State::WAITING_FOR_GAME);

			if (level.m_JoinedUsers.size() >= level.m_MaxPlayers)
			{
				startLevel(level);
			}

			break;
		}
	}
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

void Lobby::handlePackages()
{
	for(auto& wUser : m_FreeUsers)
	{
		handlePackagesForOneUser(wUser);
	}
}

void Lobby::handlePackagesForOneUser(User::wPtr p_User)
{
	User::ptr user = p_User.lock();

	if (!user)
	{
		return;
	}

	IConnectionController* con = user->getConnection();

	unsigned int numPackages = con->getNumPackages();
	for (unsigned int i = 0; i < numPackages; ++i)
	{
		Package package = con->getPackage(i);
		PackageType type = con->getPackageType(package);

		switch (type)
		{
		case PackageType::JOIN_GAME:
			{
				std::string levelName = con->getJoinGameName(package);
				joinLevel(user, levelName);

				con->clearPackages(i + 1);
				return;
			}

		default:
			std::string msg("Received unhandled package of type " + std::to_string((uint16_t)type));
			Logger::log(Logger::Level::WARNING, msg);
			break;
		}
	}

	con->clearPackages(numPackages);
}
