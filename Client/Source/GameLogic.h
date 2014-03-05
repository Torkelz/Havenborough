#pragma once
#include <Actor.h>
#include "ActorFactory.h"
#include "ActorList.h"
#include "Level.h"
#include "Player.h"
#include "EdgeCollisionResponse.h"
#include "EventManager.h"
#include "Input/Input.h"

#include "SpellFactory.h"
#include "PhysicsTypes.h"

#include <INetwork.h>

class GameLogic
{
public:
	enum class GoToScene
	{
		NONE = -1,
		POSTGAME = 2
	};

private:
	IPhysics *m_Physics;
	ResourceManager *m_ResourceManager;
	INetwork *m_Network;
	EventManager *m_EventManager;

	Level m_Level;
	Player m_Player;
	std::string m_LevelName;
	std::string m_Username;
	
	EdgeCollisionResponse m_EdgeCollResponse;
	
	std::vector<int> m_ResourceIDs;

	GoToScene m_ChangeScene;

	ActorFactory* m_ActorFactory;
	ActorList::ptr m_Actors;

	Actor::wPtr m_PlayerSparks;

	Actor::wPtr m_FlyingCamera;
	Actor::wPtr m_SplineCamera;
	Actor::wPtr m_PlayerDefault;

	bool m_IsConnecting;
	bool m_Connected;
	bool m_InGame;
	bool m_PlayingLocal;
	bool m_StartLocal;
	float m_CountdownTimer;
	bool m_RenderGo;

	unsigned int m_PlayerPositionInRace;
	float m_PlayerTimeDifference; //The difference in time to the first player.

	//DEBUG
	Vector3 m_PlayerDirection;
public:
	GameLogic(void);
	~GameLogic(void);

	void initialize(ResourceManager *p_ResourceManager,	IPhysics *p_Physics, ActorFactory *p_ActorFactory,
		EventManager *p_EventManager, INetwork *p_Network); 
	void shutdown(void);

	ActorList::ptr getObjects();
	
	/**
	* Gets which scene the game should change to.
	*/
	GoToScene getChangeScene(void) const;

	void onFrame(float p_DeltaTime);

	void setPlayerDirection(Vector3 p_Direction);
	Vector3 getPlayerDirection() const;
	BodyHandle getPlayerBodyHandle() const;
	Vector3 getPlayerEyePosition() const;
	Vector3 getPlayerViewRotation() const;
	Vector3 getPlayerViewForward() const;
	Vector3 getPlayerViewUp() const;
	Vector3 getPlayerViewRight() const;
	Vector3 getPlayerRotation() const;
	DirectX::XMFLOAT4X4 getPlayerViewRotationMatrix() const;
	void movePlayerView(float p_Yaw, float p_Pitch);

	IPhysics *getPhysics() const;

	void playerJump();
	void playLocalLevel();

	void connectToServer(const std::string& p_URL, unsigned short p_Port,
		const std::string& p_LevelName, const std::string& p_Username);
	void leaveGame();

	/**
	 * Called when a spell should be thrown, and all checks for a valid throw will be checked against the spell definition.
	 * 
	 * @param p_SpellId what spell to work with
	 */
	void throwSpell(const char *p_SpellId);

	/**
	 * A position in the race sent from the server.
	 *
	 * @return client's position 
	 */
	unsigned int getPlayerPositionInRace();
	/**
	 * The difference in time to the first player.
	 *
	 * @return the difference in time
	 */
	float getPlayerTimeDifference();
	//void releaseSpellInstance(int p_SpellId);

	/**
	 * Response to when someone presses the wave button.
	 * Sends an event to the server and the animation system.
	 */
	void playerWave();

	/**
	 * Activates the ability for the player to climb edges.
	 *
	 * @param p_State true if the player should be able to climb. false if the player should not be able to climb.
	 */
	void setPlayerClimb(bool p_State);

	/**
	 * Records a spline point. Position, look direction and up direction is saved.
	 *
	 */

	void recordSpline();
	/**
	 * Removes last added spline point.
	 *
	 */
	void removeLastSplineRecord();
	/**
	 * Clear current spline sequence.
	 *
	 */
	void clearSplineSequence();
	
private:
	void handleNetwork();
	void joinGame();
	
	static void connectedCallback(Result p_Res, void* p_UserData);

	Actor::ptr getActor(Actor::Id p_Actor);
	void removeActor(Actor::Id p_Actor);

	void removeActorByEvent(IEventData::Ptr p_Data);

	void playAnimation(Actor::ptr p_Actor, std::string p_AnimationName, bool p_Override);
	//void queueAnimation(Actor::ptr p_Actor, std::string p_AnimationName);
	//void changeAnimationWeight(Actor::ptr p_Actor, int p_Track, float p_Weight);

	std::weak_ptr<Actor> addActor(Actor::ptr p_Actor);

	void updateCountdownTimer(float p_DeltaTime);

	void changeCameraMode(unsigned int p_Mode);

	//TODO: DEBUG FUNCTIONS TO BE REMOVED BEFORE FINAL RELEASE
	void loadSandbox();
};