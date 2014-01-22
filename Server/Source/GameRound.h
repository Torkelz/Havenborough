#pragma once

#include "ActorFactory.h"
#include "Player.h"

#include <memory>
#include <thread>
#include <vector>

class GameRound
{
public:
	typedef std::shared_ptr<GameRound> ptr;

protected:
	std::thread m_RunThread;
	bool m_Running;

	ActorFactory::ptr m_ActorFactory;
	std::vector<Player> m_Players;

public:
	~GameRound();

	void initialize(ActorFactory::ptr p_ActorFactory);
	virtual void setup() {}

	void start();

	void addNewPlayer(User::wPtr p_User);
	std::vector<Player> getPlayers() const;

protected:
	virtual void sendLevel() = 0;
	virtual void updateLogic(float p_DeltaTime) {}
	virtual void handlePackages();
	virtual void sendUpdates() {}
	virtual void playerDisconnected(Player& p_DisconnectedPlayer) {}

private:
	void run();
	void sendLevelAndWait();
	void runGame();
	void checkForDisconnectedUsers();
};
