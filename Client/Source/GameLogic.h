#pragma once
#include <Actor.h>
#include "ActorFactory.h"
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
	
	EdgeCollisionResponse m_EdgeCollResponse;
	
	std::vector<int> m_ResourceIDs;

	GoToScene m_ChangeScene;

	ActorFactory* m_ActorFactory;
	std::vector<Actor::ptr> m_Objects;

	//Spells
	std::unique_ptr<SpellFactory> m_SpellFactory;
	std::vector<std::pair<std::string, SpellDefinition::ptr>>  m_SpellDefinitionList;
	std::vector<std::pair<int, SpellInstance::ptr>>  m_SpellInstanceList;
	int m_NextSpellInstanceId;

	void updateSpells(float p_DeltaTime);
	///

	bool m_Connected;
	bool m_InGame;
	bool m_PlayingLocal;

	//DEBUG
	std::weak_ptr<Actor> circleWitch;
	std::weak_ptr<Actor> testWitch;

	bool useIK;

	float witchCircleAngle;

	Vector2 m_PlayerDirection;
public:
	GameLogic(void);
	~GameLogic(void);

	void initialize(ResourceManager *p_ResourceManager,	IPhysics *p_Physics, ActorFactory *p_ActorFactory,
		EventManager *p_EventManager, INetwork *p_Network); 
	void shutdown(void);

	std::vector<Actor::ptr> &getObjects();
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

	IPhysics *getPhysics() const;

	void playerJump();
	void toggleIK();
	void testBlendAnimation();
	void testResetAnimation();
	void testLayerAnimation();
	void testResetLayerAnimation();
	void playLocalLevel();

	void connectToServer(const std::string& p_URL, unsigned short p_Port);
	void leaveGame();
	void joinGame(const std::string& p_LevelName);

	bool createSpellDefinition(const char *p_SpellId, const char* p_Filename);
	bool releaseSpellDefinition(const char *p_SpellId);

	int createSpellInstance(const char *p_SpellId);
	void releaseSpellInstance(int p_SpellId);

private:
	void handleNetwork();
	
	static void connectedCallback(Result p_Res, void* p_UserData);

	Actor::ptr getActor(Actor::Id p_Actor);
	void removeActor(Actor::Id p_Actor);

	SpellDefinition::ptr getSpellFromList(std::string p_SpellId);

	//TODO: DEBUG FUNCTIONS TO BE REMOVED BEFORE FINAL RELEASE
	void loadSandbox();
	void updateSandbox(float p_DeltaTime);

	void playAnimation(Actor::ptr p_Actor, std::string p_AnimationName, bool p_Override);
	void queueAnimation(Actor::ptr p_Actor, std::string p_AnimationName);
	void changeAnimationWeight(Actor::ptr p_Actor, int p_Track, float p_Weight);
	void updateIK();

	std::weak_ptr<Actor> addActor(Actor::ptr p_Actor);
};