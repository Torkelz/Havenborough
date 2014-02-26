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
#include "SpellInstance.h"
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

	bool m_IsConnecting;
	bool m_Connected;
	bool m_InGame;
	bool m_PlayingLocal;
	bool m_StartLocal;
	float m_CountdownTimer;
	bool m_RenderGo;

	//DEBUG
	Vector2 m_PlayerDirection;
	Vector3 m_CurrentCheckPointPosition;
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

	void setPlayerDirection(Vector2 p_Direction);
	Vector2 getPlayerDirection() const;
	BodyHandle getPlayerBodyHandle() const;
	Vector3 getPlayerEyePosition() const;
	Vector3 getPlayerViewRotation() const;
	Vector3 getPlayerViewForward() const;
	Vector3 getPlayerViewUp() const;
	Vector3 getPlayerViewRight() const;
	Vector3 getPlayerRotation() const;
	DirectX::XMFLOAT4X4 getPlayerViewRotationMatrix() const;
	void movePlayerView(float p_Yaw, float p_Pitch);
	Vector3 getCurrentCheckpointPosition(void) const;

	const float getPlayerCurrentMana(void);
	const float getPlayerPreviousMana(void);

	IPhysics *getPhysics() const;

	void playerJump();
	void playLocalLevel();

	void connectToServer(const std::string& p_URL, unsigned short p_Port,
		const std::string& p_LevelName, const std::string& p_Username);
	void leaveGame();

	void throwSpell(const char *p_SpellId);
	//void releaseSpellInstance(int p_SpellId);

	/**
	 * Activates the ability for the player to climb edges.
	 *
	 * @param p_State true if the player should be able to climb. false if the player should not be able to climb.
	 */
	void setPlayerClimb(bool p_State);
	
private:
	void handleNetwork();
	void joinGame();
	
	static void connectedCallback(Result p_Res, void* p_UserData);

	Actor::ptr getActor(Actor::Id p_Actor);
	void removeActor(Actor::Id p_Actor);

	void removeActorByEvent(IEventData::Ptr p_Data);

	void playAnimation(Actor::ptr p_Actor, std::string p_AnimationName, bool p_Override);
	void queueAnimation(Actor::ptr p_Actor, std::string p_AnimationName);
	void changeAnimationWeight(Actor::ptr p_Actor, int p_Track, float p_Weight);

	std::weak_ptr<Actor> addActor(Actor::ptr p_Actor);

	void updateCountdownTimer(float p_DeltaTime);

	//TODO: DEBUG FUNCTIONS TO BE REMOVED BEFORE FINAL RELEASE
	void loadSandbox();
};