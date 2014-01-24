/**
 * Stuff.
 */

#pragma once

#include "ActorFactory.h"
#include "Player.h"

#include <memory>
#include <thread>
#include <vector>

class GameList;
class Lobby;

/**
 * A game with game logic for one set of players.
 */
class GameRound : std::enable_shared_from_this<GameRound>
{
public:
	/**
	 * Shared pointer type.
	 */
	typedef std::shared_ptr<GameRound> ptr;
	/**
	 * Weak pointer type.
	 */
	typedef std::weak_ptr<GameRound> wPtr;

protected:
	GameList* m_ParentList;
	Lobby* m_ReturnLobby;
	std::thread m_RunThread;
	bool m_Running;
	std::string m_TypeName;

	ActorFactory::ptr m_ActorFactory;
	std::vector<Player> m_Players;

public:
	/**
	 * destructor.
	 */
	virtual ~GameRound();

	/**
	 * Initialize the game round.
	 *
	 * @param p_ActorFactory the factory to be used for any created actors
	 * @param p_ReturnLobby the lobby where leaving users should be returned
	 */
	void initialize(ActorFactory::ptr p_ActorFactory, Lobby* p_ReturnLobby);
	/**
	 * Set the game list that should be notified when the game ends.
	 *
	 * @param p_ParentList the containing game list
	 */
	void setOwningList(GameList* p_ParentList);
	/**
	 * Finish the last setup, should be called somewhere right before start.
	 */
	virtual void setup() {}

	/**
	 * Start the game round asynchronously.
	 */
	void start();

	/**
	 * Add player to the game. Should only be called before start.
	 *
	 * @param p_User the user to create player from
	 */
	void addNewPlayer(User::wPtr p_User);
	/**
	 * Get the players in the game.
	 *
	 * @return a list of the currently playing players
	 */
	std::vector<Player> getPlayers() const;

	/**
	 * Set the type identifier of the game round.
	 *
	 * @param p_TypeName a type specifying name
	 */
	void setGameType(const std::string& p_TypeName);
	/**
	 * Get the type identifier for the game round.
	 *
	 * @return the game type name
	 */
	std::string getGameType() const;

protected:
	/**
	 * Send the level information to all connected players.
	 */
	virtual void sendLevel() = 0;
	/**
	 * Update any game logic.
	 *
	 * @param p_DeltaTime the time since the last update
	 */
	virtual void updateLogic(float p_DeltaTime) {}
	/**
	 * Handle any received packages.
	 */
	virtual void handlePackages();
	/**
	 * Send any needed updates to all players.
	 */
	virtual void sendUpdates() {}
	/**
	 * Perform any neccesary action when a player disconnects.
	 * <p>
	 * The function is called after the player is disconnected but
	 * before the player is removed from the list of playing players.
	 *
	 * @param p_DisconnectedPlayer the disconnected player
	 */
	virtual void playerDisconnected(Player& p_DisconnectedPlayer) {}

private:
	void run();
	void sendLevelAndWait();
	void runGame();
	void checkForDisconnectedUsers();
};
