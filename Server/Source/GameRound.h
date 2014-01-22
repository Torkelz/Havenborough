#pragma once

#include "ActorFactory.h"
#include "Player.h"

#include <memory>
#include <thread>
#include <vector>

class GameList;

class GameRound : std::enable_shared_from_this<GameRound>
{
public:
	typedef std::shared_ptr<GameRound> ptr;

protected:
	GameList* m_ParentList;
	std::thread m_RunThread;
	bool m_Running;
	std::string m_TypeName;

	ActorFactory::ptr m_ActorFactory;
	std::vector<Player> m_Players;

public:
	~GameRound();

	void initialize(ActorFactory::ptr p_ActorFactory);
	void setOwningList(GameList* p_ParentList);
	virtual void setup() {}

	void start();

	void addNewPlayer(User::wPtr p_User);
	std::vector<Player> getPlayers() const;

	void setGameType(const std::string& p_TypeName);
	std::string getGameType() const;

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
