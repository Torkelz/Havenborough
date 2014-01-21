#pragma once

#include "GameList.h"
#include "Lobby.h"
#include "User.h"

#include <INetwork.h>

#include <tinyxml2.h>

#include <mutex>
#include <thread>
#include <vector>

class Server
{
private:

	INetwork* m_Network;

	std::unique_ptr<Lobby> m_Lobby;
	GameList m_Games;

	uint16_t m_LastActorId;
	std::vector<User::ptr> m_Users;

	bool m_RemoveBox;
	bool m_PulseObject;

	std::mutex m_UserLock;
	
	bool m_Running;
	std::thread m_UpdateThread;

public:
	Server();

	void initialize();
	void run();
	void shutdown();
	
	std::vector<std::string> getUserNames();
	void sendTestData();
	void sendPulseObject();

	void addNewGame(GameRound::ptr p_Game);

private:
	static void clientConnected(IConnectionController* p_Connection, void* p_UserData);
	static void clientDisconnected(IConnectionController* p_Connection, void* p_UserData);

	void removeLastBox();
	void pulse();
	void updateClients();
};
