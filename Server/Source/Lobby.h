#pragma once

#include "User.h"

#include <vector>

class Lobby
{
public:
	struct AvailableLevel
	{
		std::vector<User::wPtr> m_JoinedUsers;
		unsigned int m_MaxPlayers;
		std::string m_LevelName;
	};

private:
	std::vector<AvailableLevel> m_Levels;
	std::vector<User::wPtr> m_FreeUsers;

public:
	void checkFreeUsers();

	void addAvailableLevel(const std::string& p_LevelName, unsigned int p_MaxPlayers);
	void addFreeUser(User::wPtr p_User);

private:
	void startLevel(AvailableLevel& p_Level);
};
