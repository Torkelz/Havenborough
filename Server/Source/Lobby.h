#pragma once

#include "GameRoundFactory.h"
#include "User.h"

#include <vector>

class Server;

class Lobby
{
public:
	struct AvailableLevel
	{
		std::vector<User::wPtr> m_JoinedUsers;
		unsigned int m_MaxPlayers;
		std::string m_LevelName;
		float m_TimeoutLength;
		float m_WaitedTime;
	};

private:
	Server* m_Server;

	std::vector<AvailableLevel> m_Levels;
	std::vector<User::wPtr> m_FreeUsers;
	GameRoundFactory m_GameFactory;

public:
	explicit Lobby(Server* p_Server);

	void checkFreeUsers(float p_DeltaTime);

	void addAvailableLevel(const std::string& p_LevelName, unsigned int p_MaxPlayers);
	void addFreeUser(User::wPtr p_User);

private:
	void startLevel(AvailableLevel& p_Level);
};
