#pragma once
#include "IScene.h"
#include "../Logger.h"
#include "../Level.h"
#include "../Player.h"
#include "../EdgeCollisionResponse.h"

class GameScene : public IScene
{
private:
	int m_SceneID;
	bool m_Visible;
	int  m_NewSceneID;
	bool m_ChangeScene;
	bool m_ChangeList;
	
	IGraphics *m_Graphics;
	ResourceManager *m_ResourceManager;
	IPhysics *m_Physics;
	Level m_Level;
	Player m_Player;
	Input *m_InputQueue;
	EdgeCollisionResponse m_EdgeCollResponse;

	BodyHandle m_Ground;
	BodyHandle m_FinishLine;

	std::vector<int> m_ResourceIDs;

	//DEBUG
	int currentDebugView;
	int climbBox;
	int jointBox;
	int skyBox;
	int ground;
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
	GameScene();
	~GameScene();

	bool init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
		IPhysics *p_Physics, Input *p_InputQueue) override; 
	void destroy() override;

	void onFrame(float p_DeltaTime, int* p_IsCurrentScene) override;
	void render() override;

	bool getIsVisible() override;
	void setIsVisible(bool p_SetVisible) override;

	void registeredInput(std::string p_Action, float p_Value) override;

	int	getID() override;

private:
	//TODO: DEBUG FUNCTIONS TO BE REMOVED BEFORE FINAL RELEASE
	void loadSandbox();
	void updateSandbox(float p_DeltaTime);
	void renderSandbox();
	void shutdownSandbox();
};