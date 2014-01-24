#pragma once
#include "Actor.h"
#include "ActorFactory.h"
#include "Logger.h"
#include "Level.h"
#include "Player.h"
#include "EdgeCollisionResponse.h"
#include "EventManager.h"
#include "Input/Input.h"

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
	std::weak_ptr<Actor> m_FinishLine;

	GoToScene m_ChangeScene;

	ActorFactory* m_ActorFactory;
	std::vector<Actor::ptr> m_Objects;

	bool m_Connected;

	//DEBUG
	std::weak_ptr<Actor> circleWitch;
	std::weak_ptr<Actor> standingWitch;
	std::weak_ptr<Actor> wavingWitch;
	std::weak_ptr<Actor> ikTest;
	std::weak_ptr<Actor> testWitch;
	std::weak_ptr<Actor> testCheckpoint;

	const static int NUM_BOXES = 16;
	std::weak_ptr<Actor> rotBoxes[NUM_BOXES];
	bool useIK;
	Vector3 rotBlockRotation;

	Vector3 lookDir;
	float witchCircleAngle;

	Vector2 m_PlayerDirection;
	Vector3 m_PlayerViewRotation;

public:
	GameLogic(void);
	~GameLogic(void);

	void initialize(ResourceManager *p_ResourceManager,	IPhysics *p_Physics, ActorFactory *p_ActorFactory, EventManager *p_EventManager, INetwork *p_Network); 
	void shutdown(void);

	std::vector<Actor::ptr> &getObjects();
	/**
	* Gets which scene the game should change to.
	*/
	GoToScene getChangeScene(void) const;

	void onFrame(float p_DeltaTime);

	void setPlayerActor(std::weak_ptr<Actor> p_Actor);

	void setPlayerDirection(Vector2 p_Direction);
	Vector2 getPlayerDirection() const;
	BodyHandle getPlayerBodyHandle() const;
	Vector3 getPlayerEyePosition() const;
	Vector3 getPlayerViewRotation() const;
	void movePlayerView(float p_Yaw, float p_Pitch);

	IPhysics *getPhysics() const;

	void playerJump();
	void toggleIK();
	void testBlendAnimation();
	void testResetAnimation();
	void testLayerAnimation();
	void testResetLayerAnimation();

private:
	void handleNetwork();
	
	static void connectedCallback(Result p_Res, void* p_UserData);

	Actor::ptr getActor(Actor::Id p_Actor);
	void removeActor(Actor::Id p_Actor);

	//TODO: DEBUG FUNCTIONS TO BE REMOVED BEFORE FINAL RELEASE
	void loadSandbox();
	void updateSandbox(float p_DeltaTime);

	void playAnimation(Actor::ptr p_Actor, std::string p_AnimationName);
	void updateIK();

	std::weak_ptr<Actor> addRotatingBox(Vector3 p_Position, Vector3 p_Scale);
	std::weak_ptr<Actor> addSkybox(Vector3 p_Scale);
	std::weak_ptr<Actor> addBasicModel(const std::string& p_Model, Vector3 p_Position);
	std::weak_ptr<Actor> addIK_Worm();
	std::weak_ptr<Actor> addBoxWithAABB(Vector3 p_Position, Vector3 p_Halfsize);
	std::weak_ptr<Actor> addBoxWithOBB(Vector3 p_Position, Vector3 p_Halfsize, Vector3 p_Rotation);
	std::weak_ptr<Actor> addClimbBox();
	std::weak_ptr<Actor> addClimbTowerBox(Vector3 p_Position, Vector3 p_Halfsize);
	std::weak_ptr<Actor> addCollisionSphere(Vector3 p_Position, float p_Radius);
	std::weak_ptr<Actor> addCheckPointActor(Vector3 p_Position, Vector3 p_Color);
	std::weak_ptr<Actor> addPlayerActor(Vector3 p_Position);


	void addLights();
};