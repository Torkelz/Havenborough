#include "Lobby.h"

void Lobby::checkFreeUsers()
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

			if (level.m_JoinedUsers.size() == level.m_MaxPlayers)
			{
				startLevel(level);
			}

			break;
		}
	}

	m_FreeUsers.clear();
}

void Lobby::addAvailableLevel(const std::string& p_LevelName, unsigned int p_MaxPlayers)
{
	AvailableLevel level =
	{
		std::vector<User::wPtr>(),
		p_MaxPlayers,
		p_LevelName
	};
	m_Levels.push_back(level);
}

void Lobby::addFreeUser(User::wPtr p_User)
{
	m_FreeUsers.push_back(p_User);
}

void Lobby::startLevel(AvailableLevel& p_Level)
{
	p_Level.m_JoinedUsers.clear();
}
