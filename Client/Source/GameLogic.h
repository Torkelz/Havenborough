#pragma once
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
	IGraphics *m_Graphics;
	IPhysics *m_Physics;
	Input *m_InputQueue;
	ResourceManager *m_ResourceManager;

	Level m_Level;
	int skyBox;
	int ground;
	Player m_Player;
	EdgeCollisionResponse m_EdgeCollResponse;
	
	std::vector<int> m_ResourceIDs;
	BodyHandle m_Ground;
	BodyHandle m_FinishLine;

	GoToScene m_ChangeScene;

	//DEBUG
	int currentDebugView;
	int climbBox;
	int jointBox;
	int circleWitch;
	int standingWitch;
	int wavingWitch;
	int ikTest;
	int slantedPlane;
	int OBBhouse1;
	int OBBhouse2;
	const static int NUM_BOXES = 16;
	int boxIds[NUM_BOXES];
	bool useIK_OnIK_Worm;
	float yaw;
	float yawSpeed;
	float pitch;
	float pitchSpeed;
	float roll;
	float rollSpeed;
	static const unsigned int numTowerBoxes = 5;
	int towerBoxes[numTowerBoxes];
	static const unsigned int numRotatedTowerBoxes = 5;
	int rotatedTowerBoxes[numRotatedTowerBoxes];
	float viewRot[2];
	float sensitivity;
	float witchCircleAngle;
public:
	GameLogic(void);
	~GameLogic(void);

	void initialize(IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
		IPhysics *p_Physics, Input *p_InputQueue); 
	void shutdown(void);

	/**
	* Gets which scene the game should change to.
	*/
	GoToScene getChangeScene(void) const;

	void onFrame(float p_DeltaTime);
	void render(void);

	void registeredInput(std::string p_Action, float p_Value);

private:
	//TODO: DEBUG FUNCTIONS TO BE REMOVED BEFORE FINAL RELEASE
	void loadSandbox();
	void updateSandbox(float p_DeltaTime);
	void renderSandbox();
	void shutdownSandbox();

	void addDebugBVToDraw(BodyHandle p_BodyHandle);
};