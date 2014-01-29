#include "GameRound.h"

#include "GameList.h"
#include "Lobby.h"
#include "../../Client/Source/Logger.h"

#include <algorithm>

GameRound::~GameRound()
{
	m_Running = false;

	m_ParentList->removeGameRound();
	if (m_RunThread.joinable())
	{
		m_RunThread.detach();
	}
}

void GameRound::initialize(ActorFactory::ptr p_ActorFactory, Lobby* p_ReturnLobby)
{
	m_ActorFactory = p_ActorFactory;
	m_ReturnLobby = p_ReturnLobby;
}

void GameRound::setOwningList(GameList* p_ParentList)
{
	m_ParentList = p_ParentList;
}

void GameRound::start()
{
	m_RunThread = std::thread(&GameRound::run, shared_from_this());
}

void GameRound::addNewPlayer(User::wPtr p_User)
{
	m_Players.push_back(Player(p_User));
}

std::vector<Player> GameRound::getPlayers() const
{
	return m_Players;
}

void GameRound::setGameType(const std::string& p_TypeName)
{
	m_TypeName = p_TypeName;
}

std::string GameRound::getGameType() const
{
	return m_TypeName;
}

void GameRound::handlePackages()
{
	for(auto& player : m_Players)
	{
		User::ptr user = player.getUser().lock();

		if (!user)
		{
			continue;
		}

		IConnectionController* con = user->getConnection();

		unsigned int numPackages = con->getNumPackages();
		for (unsigned int i = 0; i < numPackages; ++i)
		{
			Package package = con->getPackage(i);
			PackageType type = con->getPackageType(package);

			switch (type)
			{
			case PackageType::PLAYER_CONTROL:
				{
					PlayerControlData playerControlData = con->getPlayerControlData(package);
					player.m_PlayerBox.position.x = playerControlData.m_Velocity[0];
					player.m_PlayerBox.position.y = playerControlData.m_Velocity[1];
					player.m_PlayerBox.position.z = playerControlData.m_Velocity[2];
					//player.m_PlayerBox.velocity.x = playerControlData.m_Velocity[0];
					//player.m_PlayerBox.velocity.y = playerControlData.m_Velocity[1];
					//player.m_PlayerBox.velocity.z = playerControlData.m_Velocity[2];
					player.m_PlayerBox.rotation.x = playerControlData.m_Rotation[0];
					player.m_PlayerBox.rotation.y = playerControlData.m_Rotation[1];
					player.m_PlayerBox.rotation.z = playerControlData.m_Rotation[2];
				}
				break;

			case PackageType::DONE_LOADING:
				{
					user->setState(User::State::WAITING_FOR_START);
				}
				break;

			case PackageType::LEAVE_GAME:
				{
					m_ReturnLobby->addFreeUser(user);
					player.releaseUser();
				}
				break;

			default:
				std::string msg("Received unhandled package of type " + std::to_string((uint16_t)type));
				Logger::log(Logger::Level::WARNING, msg);
				break;
			}
		}

		con->clearPackages(numPackages);
	}
}

void GameRound::run()
{
	Logger::log(Logger::Level::INFO, "Starting game round");
	m_Running = true;

	try
	{
		sendLevelAndWait();

		if (m_Players.empty())
		{
			Logger::log(Logger::Level::INFO, "All clients disconnected before level loaded, aborting game round");
			return;
		}

		Logger::log(Logger::Level::INFO, "Level loaded by clients, starting game");

		runGame();
	}
	catch (std::exception& ex)
	{
		Logger::log(Logger::Level::FATAL, std::string("Unexpected exception stopped game: ") + ex.what());
	}
	catch (...)
	{
		Logger::log(Logger::Level::FATAL, "Unexpected exception stopped game round");
	}

	Logger::log(Logger::Level::INFO, "Game round stopped");
}

void GameRound::sendLevelAndWait()
{
	bool allDoneLoading = false;

	for (auto& player : m_Players)
	{
		User::ptr user = player.getUser().lock();
		if (user)
		{
			user->setState(User::State::LOADING_LEVEL);
		}
	}

	sendLevel();

	while (m_Running && !allDoneLoading)
	{
		allDoneLoading = true;

		handlePackages();

		for (auto& player : m_Players)
		{
			User::ptr user = player.getUser().lock();

			if (!user)
			{
				continue;
			}

			if (user->getState() != User::State::WAITING_FOR_START)
			{
				allDoneLoading = false;
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	checkForDisconnectedUsers();
}

void GameRound::runGame()
{
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point previousTime;
	float deltaTime = 0.001f;

	while (m_Running)
	{
		previousTime = currentTime;
		currentTime = std::chrono::high_resolution_clock::now();
		const std::chrono::high_resolution_clock::duration frameTime = currentTime - previousTime;

		handlePackages();
		checkForDisconnectedUsers();
		updateLogic(deltaTime);
		sendUpdates();

		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(frameTime).count();

		static const std::chrono::milliseconds sleepDuration(20);
		std::this_thread::sleep_for(sleepDuration - frameTime);
	}
}

void GameRound::checkForDisconnectedUsers()
{
	auto split = std::partition(m_Players.begin(), m_Players.end(),
		[] (const Player& p_Player)
		{
			return p_Player.getUser().lock();
		});

	for (auto removePlayer = split; removePlayer != m_Players.end(); ++removePlayer)
	{
		playerDisconnected(*removePlayer);
	}

	m_Players.erase(split, m_Players.end());

	if (m_Players.empty())
	{
		m_Running = false;
	}
}
