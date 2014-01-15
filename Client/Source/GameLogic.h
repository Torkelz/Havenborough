#pragma once
#include "Actor.h"
#include "ActorFactory.h"
#include "Logger.h"
#include "Level.h"
#include "Player.h"
#include "EdgeCollisionResponse.h"
#include "Input/Input.h"

class GameLogic
{
public:
	enum class GoToScene
	{
		NONE = -1,
		POSTGAME = 2
	};

private:
	//IGraphics *m_Graphics;
	IPhysics *m_Physics;
	//Input *m_InputQueue;
	ResourceManager *m_ResourceManager;

	Level m_Level;
	Player m_Player;
	
	std::weak_ptr<Actor> m_PlayerActor;
	EdgeCollisionResponse m_EdgeCollResponse;
	
	std::vector<int> m_ResourceIDs;
	BodyHandle m_Ground;
	BodyHandle m_FinishLine;

	GoToScene m_ChangeScene;

	ActorFactory* m_ActorFactory;
	std::vector<Actor::ptr> m_Objects;

	//DEBUG
	int climbBox;
	int jointBox;
	std::weak_ptr<Actor> circleWitch;
	std::weak_ptr<Actor> skyBox;
	int standingWitch;
	int wavingWitch;
	int ikTest;
	int slantedPlane;
	int OBBhouse1;
	int OBBhouse2;
	const static int NUM_BOXES = 16;
	std::weak_ptr<Actor> rotBoxes[NUM_BOXES];
	bool useIK_OnIK_Worm;
	Vector3 rotBlockRotation;
	static const unsigned int numTowerBoxes = 5;
	int towerBoxes[numTowerBoxes];
	static const unsigned int numRotatedTowerBoxes = 5;
	int rotatedTowerBoxes[numRotatedTowerBoxes];
	float viewRot[2];
	Vector3 lookDir;
	float witchCircleAngle;

	Vector2 m_PlayerDirection;

public:
	GameLogic(void);
	~GameLogic(void);

	void initialize(ResourceManager *p_ResourceManager,	IPhysics *p_Physics, ActorFactory *p_ActorFactory); 
	void shutdown(void);

	std::vector<Actor::ptr> &getObjects();
	/**
	* Gets which scene the game should change to.
	*/
	GoToScene getChangeScene(void) const;

	void onFrame(float p_DeltaTime);
	//void render(void);

	//void registeredInput(std::string p_Action, float p_Value);

	void setPlayerActor(std::weak_ptr<Actor> p_Actor);

	Vector3 getPlayerViewRotation();

	void setPlayerDirection(Vector2 p_Direction);
	Vector2 getPlayerDirection() const;

	Vector3 getPlayerEyePosition() const;
	Vector3 getPlayerViewRotation() const;
	void movePlayerView(float p_Yaw, float p_Pitch);

	void playerJump();
	void toggleIK();

private:
	//TODO: DEBUG FUNCTIONS TO BE REMOVED BEFORE FINAL RELEASE
	void loadSandbox();
	void updateSandbox(float p_DeltaTime);
	void renderSandbox();
	void shutdownSandbox();

	void addDebugBVToDraw(BodyHandle p_BodyHandle);

	std::weak_ptr<Actor> addRotatingBox(Vector3 p_Position, Vector3 p_Scale);
	std::weak_ptr<Actor> addSkybox(Vector3 p_Scale);
	std::weak_ptr<Actor> addDzala(Vector3 p_Position);
	std::weak_ptr<Actor> addIK_Worm();
	std::weak_ptr<Actor> addBoxWithAABB(Vector3 p_Position, Vector3 p_Halfsize);
	std::weak_ptr<Actor> addBoxWithOBB(Vector3 p_Position, Vector3 p_Halfsize, Vector3 p_Rotation);
};