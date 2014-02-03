#include "Server.h"

#include <Logger.h>

Server::Server()
	:	m_LastActorId(0),
		m_RemoveBox(false),
		m_PulseObject(false)
{
}

void Server::initialize()
{
	m_Running = false;

	m_Lobby.reset(new Lobby(this));
	m_Lobby->addAvailableLevel("test", 4);
	m_Lobby->addAvailableLevel("serverLevel", 8);
	m_Lobby->addAvailableLevel("serverDebugLevel", 4);
	m_Network = INetwork::createNetwork();
	m_Network->initialize();
	m_Network->createServer(31415);
	m_Network->setClientConnectedCallback(&Server::clientConnected, this);
	m_Network->setClientDisconnectedCallback(&Server::clientDisconnected, this);
	m_Network->startServer(3);
}

void Server::run()
{
	m_Running = true;
	m_UpdateThread = std::thread(&Server::updateClients, this);
}

void Server::shutdown()
{
	std::lock_guard<std::mutex> lock(m_UserLock);

	m_Running = false;
	
	m_Network->setClientConnectedCallback(nullptr, nullptr);
	m_Network->setClientDisconnectedCallback(nullptr, nullptr);

	m_Lobby.reset();
	m_Games.stopAllGames();

	m_UpdateThread.join();
	INetwork::deleteNetwork(m_Network);
}

std::vector<std::string> Server::getUserNames()
{
	std::vector<std::string> names;

	std::lock_guard<std::mutex> lock(m_UserLock);

	for (auto& user : m_Users)
	{
		names.push_back(std::to_string((intptr_t)user->getConnection()));
	}

	return names;
}

std::vector<std::string> Server::getGameDescriptions()
{
	std::vector<std::string> descriptions;

	for (const auto& game : m_Games.getRunningGames())
	{
		descriptions.push_back("Game \"" + game->getGameType() + "\" with " + std::to_string(game->getPlayers().size()) + " players");
	}

	return descriptions;
}

void Server::sendTestData()
{
	m_RemoveBox = true;
}

void Server::sendPulseObject()
{
	m_PulseObject = true;
}

void Server::addNewGame(GameRound::ptr p_Game)
{
	m_Games.addGameRound(p_Game);
}

void Server::clientConnected(IConnectionController* p_Connection, void* p_UserData)
{
	Logger::log(Logger::Level::INFO, "Client connected");

	Server* obj = static_cast<Server*>(p_UserData);

	User::ptr user(new User(p_Connection));
	obj->m_Users.push_back(user);

	obj->m_Lobby->addFreeUser(user);
}

void Server::clientDisconnected(IConnectionController* p_Connection, void* p_UserData)
{
	Logger::log(Logger::Level::INFO, "Client disconnected");

	Server* obj = static_cast<Server*>(p_UserData);

	std::lock_guard<std::mutex> lock(obj->m_UserLock);

	for(unsigned int i = 0; i < obj->m_Users.size(); i++)
	{
		if(obj->m_Users[i]->getConnection() == p_Connection)
		{
			obj->m_Users.erase(obj->m_Users.begin() + i);
			break;
		}
	}
}

void Server::removeLastBox()
{
	//if (m_Boxes.empty())
	//{
	//	return;
	//}

	//const uint16_t objectsToRemove[] = { m_Boxes.back().actorId };
	//m_Boxes.pop_back();

	//std::lock_guard<std::mutex> lock(m_UserLock);
	//for(auto& player : m_Players)
	//{
	//	player.m_Connection->getConnection()->sendRemoveObjects(objectsToRemove, 1);
	//}
}

void Server::pulse()
{
	//if (m_Boxes.empty())
	//{
	//	return;
	//}

	//const uint16_t object = m_Boxes.front().actorId;

	//tinyxml2::XMLPrinter printer;
	//printer.OpenElement("Action");
	//printer.OpenElement("Pulse");
	//printer.CloseElement();
	//printer.CloseElement();

	//std::lock_guard<std::mutex> lock(m_UserLock);
	//for(auto& player : m_Players)
	//{
	//	player.m_Connection->getConnection()->sendObjectAction(object, printer.CStr());
	//}
}

void Server::updateClients()
{
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previousTime;
	float deltaTime = 0.001f;

	while (m_Running)
	{
		m_Lobby->checkFreeUsers(deltaTime);

		if (m_RemoveBox)
		{
			removeLastBox();
			m_RemoveBox = false;
		}

		if (m_PulseObject)
		{
			pulse();
			m_PulseObject = false;
		}
		
		previousTime = currentTime;
		currentTime = std::chrono::high_resolution_clock::now();
		const std::chrono::high_resolution_clock::duration frameTime = currentTime - previousTime;

		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(frameTime).count();

		static const std::chrono::milliseconds sleepDuration(20);
		std::this_thread::sleep_for(sleepDuration - frameTime);
	}
}