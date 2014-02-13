/**
 * Stuff.
 */

#pragma once

#include "GameList.h"
#include "Lobby.h"
#include "User.h"

#include <INetwork.h>

#include <tinyxml2/tinyxml2.h>

#include <mutex>
#include <thread>
#include <vector>

/**
 * A multiplayer, multi-game server.
 */
class Server
{
private:
	INetwork* m_Network;

	std::unique_ptr<Lobby> m_Lobby;
	GameList m_Games;

	std::vector<User::ptr> m_Users;

	bool m_RemoveBox;
	bool m_PulseObject;

	std::mutex m_UserLock;
	
	bool m_Running;
	std::thread m_UpdateThread;

public:
	/**
	 * contructor.
	 */
	Server();

	/**
	 * Initialize the server and start listening for clients.
	 */
	void initialize();
	/**
	 * Start the server logic for managing clients.
	 */
	void run();
	/**
	 * Disconnect all users and shutdown the server.
	 */
	void shutdown();
	
	/**
	 * Get the names of all connected users.
	 *
	 * @return a list of user names
	 */
	std::vector<std::string> getUserNames();
	/**
	 * Get the descriptions for all running games.
	 *
	 * @return game descriptions
	 */
	std::vector<std::string> getGameDescriptions();
	/**
	 * Send some test data.
	 */
	void sendTestData();
	/**
	 * Pulse a object on the client.
	 */
	void sendPulseObject();

	/**
	 * Add a new game to the list of running games.
	 *
	 * @param p_Game the game to run
	 */
	void addNewGame(GameRound::ptr p_Game);

private:
	static void clientConnected(IConnectionController* p_Connection, void* p_UserData);
	static void clientDisconnected(IConnectionController* p_Connection, void* p_UserData);

	void removeLastBox();
	void pulse();
	void updateClients();
};
